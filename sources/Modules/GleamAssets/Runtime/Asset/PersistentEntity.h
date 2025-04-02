#pragma once
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/World/World.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    /**
     * Entity持久化包装器，用Entity模拟出正常对象一样用指针引用、序列化、构造析构的功能，从而使其能被序列化
     *
     * 每当一个Entity与资源包有连接时，EntityAsset就会存在，用于管理Entity与AssetRef的关联信息。
     * 资源卸载后EntityAsset也会销毁，因此可以用EntityAsset是否存在来判断，Entity是否与资源包相连
     */
    class PersistentEntity
    {
    public:
        static std::optional<std::reference_wrapper<PersistentEntity>> GetEntityAsset(Entity entity);

        PersistentEntity();
        PersistentEntity(Entity entity, bool ownership);
        PersistentEntity(PersistentEntity&& other) noexcept;
        PersistentEntity& operator=(PersistentEntity&& other) noexcept;
        ~PersistentEntity();

        Entity GetEntity() const;
        bool GetOwnership() const;
        void SetEntity(Entity entity);
        void SetOwnership(bool ownership);

    private:
        inline static std::unordered_map<Entity, PersistentEntity*> entityToAsset = {};

        Entity entity;
        bool ownership;
    };
    //Entity持久化函数
    Gleam_MakeTypeWithID(PersistentEntity, "112887C5-1B8D-42DF-801D-4360DA6F8A15")
    {
        if constexpr (std::derived_from<TFieldTransferrer, FieldDataTransferrer>)
        {
            FieldDataTransferrer& dataTransferrer = transferrer;

            const Archetype* archetype;
            std::byte* components;

            if (value.GetEntity() != Entity::Null) //持久化 
            {
                const EntityInfo& entityInfo = World::GetEntityInfoAllocator().GetEntityInfo(value.GetEntity());
                archetype = entityInfo.archetype;
                components = entityInfo.memoryAddress;

                //持久化原型
                int componentCount = archetype->GetComponentCount();
                std::vector<uuids::uuid> componentTypes = std::vector<uuids::uuid>(componentCount);
                for (std::size_t i = 0; i < componentCount; ++i)
                    componentTypes[i] = archetype->GetComponentType(i).GetID();
                dataTransferrer.TransferField("componentTypes", componentTypes);
            }
            else //反持久化（EntityAsset除了反序列化时，逻辑上不允许创建空置，因此为空时一定是反序列化）
            {
                //反持久化原型
                std::vector<uuids::uuid> componentTypeIDs;
                dataTransferrer.TransferField("componentTypes", componentTypeIDs);
                std::vector<std::reference_wrapper<const Type>> componentTypes;
                for (auto& componentTypeID : componentTypeIDs)
                {
                    std::optional<std::reference_wrapper<const Type>> optionalType = Type::GetType(componentTypeID);
                    if (optionalType.has_value())
                        componentTypes.push_back(optionalType.value());
                }

                archetype = &Archetype::CreateOrGet(componentTypes);
                value = {World::AddEntity(*archetype), true};
                components = World::GetEntityInfoAllocator().GetEntityInfo(value.GetEntity()).memoryAddress;
            }

            //序列化组件
            for (std::size_t i = 0; i < archetype->GetComponentCount(); ++i)
            {
                const Type& type = archetype->GetComponentType(i);
                void* component = components + archetype->GetComponentOffset(i);

                dataTransferrer.PushNode(std::format("component_{}", i), DataType::Class);
                type.Serialize(dataTransferrer, component);
                dataTransferrer.PopNode();
            }
        }
    }

    /**
     *  将Entity引用包装成资源引用进行传输，从而使Entity引用可以被持久化
     */
    template <>
    inline void FieldDataTransferrer::Transfer<Entity>(Entity& value)
    {
        //将实体引用视作AssetRef进行传输
        if (
            dynamic_cast<Serializer*>(this) ||
            dynamic_cast<PointerSerializer*>(this) ||
            dynamic_cast<AssetRefStatistician*>(this)
        )
        {
            //获取引用实体的资源指针
            auto optionalEntityAsset = PersistentEntity::GetEntityAsset(value);
            PersistentEntity* entityAsset = optionalEntityAsset.has_value() ? &optionalEntityAsset.value().get() : nullptr;

            AssetRef assetRef = AssetBundle::pointerToAssetRef[&value]; //读取来自首次反序列化时保存的值或默认空值
            {
                //优先利用资源指针获取目标对象的准确资源地址（序列化时保存），否则使用指针映射表存储的资源地址（首次反序列时保存）
                assetRef = AssetBundle::GetAssetRef(entityAsset).value_or(assetRef);
                assert(entityAsset == nullptr || !assetRef.assetBundleID.is_nil() && "引用的实体未被持久化！");
                Transfer(assetRef); //序列化时写入或首次反序列化时从文件读取（PointerSerializer不执行传输）
                //根据资源依赖获取数据
                std::shared_ptr<void> object = AssetBundle::GetObject(assetRef).value_or(std::shared_ptr<void>{});
                value = object == nullptr ? Entity::Null : static_cast<PersistentEntity*>(object.get())->GetEntity();
            }
            AssetBundle::pointerToAssetRef[&value] = assetRef; //首次反序列化结束时保存来自资源文件的值
        }
        //统计未托管的可持久化实体
        else if (ObjectRefStatistician* statistician = dynamic_cast<ObjectRefStatistician*>(this))
        {
            if (value == Entity::Null || PersistentEntity::GetEntityAsset(value).has_value())
                return;

            statistician->dependencies.emplace_back(PersistentEntity{value, false});
        }
        //默认传输方式
        else
        {
            FallbackTransfer(&value, typeid(value));
        }
    }
}