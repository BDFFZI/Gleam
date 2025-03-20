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
    class EntityAsset
    {
    public:
        static std::optional<std::reference_wrapper<EntityAsset>> GetEntityAsset(Entity entity);

        EntityAsset();
        EntityAsset(Entity entity, bool ownership);
        EntityAsset(EntityAsset&& other) noexcept;
        EntityAsset& operator=(EntityAsset&& other) noexcept;
        ~EntityAsset();

        Entity GetEntity() const;
        bool GetOwnership() const;
        void SetEntity(Entity entity);
        void SetOwnership(bool ownership);

    private:
        inline static std::unordered_map<Entity, EntityAsset*> entityToAsset = {};

        Entity entity;
        bool ownership;
    };
    //Entity持久化函数
    Gleam_MakeType(EntityAsset, "112887C5-1B8D-42DF-801D-4360DA6F8A15")
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
}

// 引导编译器编译序列化实体资源所需的模板特化
// ReSharper disable once CppUnusedIncludeDirective
#include "EntityAssetTransferrer.h"