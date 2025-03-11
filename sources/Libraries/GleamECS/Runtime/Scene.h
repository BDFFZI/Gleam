#pragma once
#include <unordered_set>

#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/System.h"
#include "GleamECS/Runtime/World.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    //场景基本信息包装器
    struct SceneAsset
    {
        std::string name;
        std::vector<uuids::uuid> systems;
    };
    Gleam_MakeType(SceneAsset, "00000000-0000-0000-0000-000000000001")
    {
        Gleam_MakeType_AddField(name);
        Gleam_MakeType_AddField(systems);
    }

    //Entity包装器，是Entity在资源包中的替身，使Entity以及Entity引用可以被资源化
    class EntityAsset
    {
    public:
        static std::optional<std::reference_wrapper<EntityAsset>> GetEntityAsset(const Entity entity)
        {
            return entityToAsset.contains(entity)
                       ? std::optional<std::reference_wrapper<EntityAsset>>{*entityToAsset.at(entity)}
                       : std::nullopt;
        }

        EntityAsset(): entity(Entity::Null)
        {
        }
        EntityAsset(const Entity entity) : entity(entity)
        {
            if (entity != Entity::Null)
                entityToAsset[entity] = this;
        }
        EntityAsset(EntityAsset&& other) noexcept
        {
            entity = other.entity;
            other.entity = Entity::Null;

            if (entity != Entity::Null)
                entityToAsset[entity] = this;
        }
        EntityAsset& operator=(EntityAsset&& other) noexcept
        {
            if (entity != Entity::Null)
                entityToAsset.erase(entity);

            entity = other.entity;
            other.entity = Entity::Null;

            if (entity != Entity::Null)
                entityToAsset[entity] = this;

            return *this;
        }
        ~EntityAsset()
        {
            if (entity != Entity::Null)
            {
                entityToAsset.erase(entity);
            }
        }

        Entity GetEntity() const
        {
            return entity;
        }
        void SetEntity(const Entity entity)
        {
            if (this->entity != Entity::Null)
                entityToAsset.erase(this->entity);

            this->entity = entity;

            if (this->entity != Entity::Null)
                entityToAsset[this->entity] = this;
        }

    private:
        inline static std::unordered_map<Entity, EntityAsset*> entityToAsset = {};

        Entity entity;
    };
    //Entity资源化函数
    Gleam_MakeType(EntityAsset, "00000000-0000-0000-0000-000000000002")
    {
        if constexpr (std::derived_from<TFieldTransferrer, FieldDataTransferrer>)
        {
            FieldDataTransferrer& dataTransferrer = transferrer;

            const Archetype* archetype;
            std::byte* components;

            if (value.GetEntity() != Entity::Null) //持久化 
            {
                const EntityInfo& entityInfo = World::GetEntityInfo(value.GetEntity());
                archetype = entityInfo.archetype;
                components = entityInfo.components;

                //持久化原型
                int componentCount = archetype->GetComponentCount();
                std::vector<uuids::uuid> componentTypes = std::vector<uuids::uuid>(componentCount);
                for (std::size_t i = 0; i < componentCount; ++i)
                    componentTypes[i] = archetype->GetComponentType(i).GetID();
                dataTransferrer.TransferField("componentTypes", componentTypes);
            }
            else //反持久化
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
                value = World::AddEntity(*archetype);
                components = World::GetEntityInfo(value.GetEntity()).components;
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
    //Entity引用资源化函数
    template <>
    struct FieldDataTransferrer_Transfer<Entity>
    {
        /**
         * 注意！由于实体地址会变动，故该映射表仅在反序列化时有效，序列化时应使用资源替身值。
         * 若未按正确的依赖顺序加载资源包，且场景中实体发生结构性变动，那引用将丢失或发生错乱！
         */
        inline static std::unordered_map<void*, AssetRef> pointerMapping = {};

        static void Invoke(FieldDataTransferrer& serializer, Entity& value)
        {
            //获取资源化替身
            std::optional<std::reference_wrapper<EntityAsset>> optionalAsset = EntityAsset::GetEntityAsset(value);
            EntityAsset* assetPtr = optionalAsset.has_value() ? &optionalAsset.value().get() : nullptr;
            //优先获取资源化替身的资源引用（序列化时保存），否则使用指针映射表存储的资源引用（首次反序列化结束时保存）
            AssetRef assetRef = AssetBundle::GetAssetRef(assetPtr).value_or(pointerMapping[&value]);
            assert(value == Entity::Null || !assetRef.assetBundleID.is_nil() && "引用的实体未被资源化！");
            //序列化或反序列化引用信息
            serializer.Transfer(assetRef);
            pointerMapping[&value] = assetRef; //存储指针对应的引用信息，以供第二次指针遍历时依然能拿到指针引用
            //重新定位资源地址
            assetPtr = static_cast<EntityAsset*>(AssetBundle::GetObject(assetRef).value_or(nullptr));
            value = assetPtr != nullptr ? assetPtr->GetEntity() : value;
        }
    };

    /**
     * 场景是一种能将世界中的实体和系统分组托管并持久化的容器。其使用场景如下
     * 1. 托管世界中的实体，使其在场景销毁时连带销毁。
     * 2. 托管系统，使其在场景启动、停止时自动添加或移除到世界。
     * 3. 分组实体和系统，使它们拥有不同的生命周期。
     * 4. 场景能够与资源包相互转换，从而使被托管的实体、系统、实体引用能被序列化。
     */
    class Scene
    {
    public:
        static auto GetAllScenes()
        {
            return allScenes | std::views::transform([](auto& scene) { return std::reference_wrapper(*scene); });
        }
        static std::optional<std::reference_wrapper<Scene>> GetScene(System& system);
        static std::optional<std::reference_wrapper<Scene>> GetScene(Entity entity);

        static Scene& Create(std::string_view name);
        static void Destroy(Scene& scene);
        static void Destroy(std::string_view name);
        static void Clear();

        static std::optional<std::reference_wrapper<Scene>> GetScene(std::string_view name);
        static void ToAssetBundle(const Scene& scene, AssetBundle& assetBundle);
        static Scene& FromAssetBundle(AssetBundle& assetBundle);

        ~Scene();

        const std::string& GetName() const
        {
            return name;
        }
        const std::unordered_set<System*>& GetSystems() const
        {
            return systems;
        }
        const std::unordered_set<Entity>& GetEntities() const
        {
            return entities;
        }
        bool GetIsRunning() const
        {
            return isRunning;
        }
        bool HasSystem(System& system) const
        {
            return systems.contains(&system);
        }
        bool HasEntity(const Entity entity) const
        {
            return entities.contains(entity);
        }

        void Start();
        void Stop();

        void AddSystem(System& system);
        void RemoveSystem(System& system);
        void AddEntity(Entity entity);
        void RemoveEntity(Entity entity);

    private:
        inline static std::vector<std::unique_ptr<Scene>> allScenes = {};
        inline static std::unordered_map<System*, Scene*> systemWorld = {};
        inline static std::unordered_map<Entity, Scene*> entityWorld = {};

        std::string name;
        std::unordered_set<System*> systems;
        std::unordered_set<Entity> entities;
        bool isRunning = false;
        // std::vector<Scene*> subScenes;
    };
}