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
    struct EntityAsset
    {
        Entity entity;

        EntityAsset(): entity(Entity::Null)
        {
        }
        EntityAsset(const Entity entity) : entity(entity)
        {
            if (entity != Entity::Null)
                entityToAsset[entity] = this;
        }
        EntityAsset(EntityAsset&& other) noexcept : entity(other.entity)
        {
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

    private:
        Gleam_MakeType_Friend
        template <class T>
        friend struct FieldDataTransferrer_Transfer;

        inline static std::unordered_map<Entity, EntityAsset*> entityToAsset = {};
    };
    //Entity资源化函数
    Gleam_MakeType(EntityAsset, "00000000-0000-0000-0000-000000000002")
    {
        if constexpr (std::derived_from<TFieldTransferrer, FieldDataTransferrer>)
        {
            FieldDataTransferrer& dataTransferrer = transferrer;

            const Archetype* archetype;
            std::byte* components;

            if (value.entity != Entity::Null) //持久化 
            {
                const EntityInfo& entityInfo = World::GetEntityInfo(value.entity);
                archetype = entityInfo.archetype;
                components = entityInfo.components;

                //持久化原型
                int componentCount = archetype->GetComponentCount() - 1;
                std::vector<uuids::uuid> componentTypes = std::vector<uuids::uuid>(componentCount);
                for (std::size_t i = 0; i < componentCount; ++i)
                    componentTypes[i] = archetype->GetComponentType(i + 1).GetID();
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
                        componentTypes.emplace_back(optionalType.value());
                }

                archetype = &Archetype::CreateOrGet(componentTypes);
                value.entity = World::AddEntity(*archetype);
                components = World::GetEntityInfo(value.entity).components;
            }

            //序列化组件
            for (std::size_t i = 1; i < archetype->GetComponentCount(); ++i)
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
        static void Invoke(FieldDataTransferrer& serializer, Entity& value)
        {
            EntityAsset* entityAsset = EntityAsset::entityToAsset.contains(value) ? EntityAsset::entityToAsset.at(value) : nullptr;

            AssetRef assetRef = AssetBundle::GetAssetRef(entityAsset).value_or(AssetRef{}); //获取引用数据对应的资源依赖
            assert(value == Entity::Null || !assetRef.assetBundleID.is_nil() && "引用的实体未被资源化！");
            serializer.Transfer(assetRef);
            entityAsset = static_cast<EntityAsset*>(AssetBundle::GetDataRef(assetRef).value_or(nullptr)); //根据资源依赖获取数据

            value = entityAsset != nullptr ? entityAsset->entity : value;
        }
    };


    /**
     * 场景是一种能将世界中的实体和系统分组并持久化的容器。
     * 其能将包含的实体和系统以及实体引用存储到磁盘中，并在需要时再加载回内存，且支持跨场景加载。
     */
    class Scene
    {
    public:
        static Scene& Create(const std::string_view name)
        {
            std::unique_ptr<Scene>& scene = allScenes.emplace_back(std::make_unique<Scene>());
            scene->name = name;
            return *scene;
        }
        static void Destroy(Scene& scene)
        {
            if (scene.isRunning) //从世界中移除系统
                scene.Stop();
            for (Entity entity : scene.entities) //从世界中移除实体
                World::RemoveEntity(entity);
            //销毁场景
            std::erase_if(allScenes, [&scene](std::unique_ptr<Scene>& scenePtr)
            {
                return scenePtr->name == scene.name;
            });
        }
        static AssetBundle& ToAssetBundle(const Scene& scene)
        {
            AssetBundle& assetBundle = AssetBundle::Create();

            //保存场景和系统信息
            SceneAsset sceneAsset;
            sceneAsset.name = scene.name;
            for (System* system : scene.systems)
                sceneAsset.systems.push_back(system->GetID());
            assetBundle.AddAsset(std::move(sceneAsset));

            //保存实体信息
            for (Entity entity : scene.entities)
            {
                EntityAsset entityAsset = {entity};
                assetBundle.AddAsset(std::move(entityAsset));
            }

            return assetBundle;
        }
        static Scene& FromAssetBundle(AssetBundle& assetBundle)
        {
            const std::vector<Asset>& assets = assetBundle.GetAssets();
            size_t assetCount = assets.size();

            //读取场景和系统信息
            SceneAsset& sceneAsset = *static_cast<SceneAsset*>(assets[0].GetDataRef());
            std::string_view name = sceneAsset.name;
            std::vector<System*> systems;
            for (auto id : sceneAsset.systems)
            {
                auto optionalSystem = System::GetSystem(id);
                if (optionalSystem.has_value())
                    systems.emplace_back(&optionalSystem.value().get());
            }


            //读取实体信息
            std::vector<Entity> entities;
            for (std::size_t i = 1; i < assetCount; ++i)
            {
                EntityAsset& entityAsset = *static_cast<EntityAsset*>(assets[i].GetDataRef());
                entities.emplace_back(entityAsset.entity);
            }

            Scene& scene = Create(name);
            scene.name = std::move(name);
            scene.systems.insert(systems.begin(), systems.end());
            scene.entities.insert(entities.begin(), entities.end());
            return scene;
        }

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

        void Start()
        {
            for (System* system : systems)
                World::AddSystem(*system);
            isRunning = true;
        }
        void Stop()
        {
            for (System* system : systems)
                World::RemoveSystem(*system);
            isRunning = false;
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

        void AddSystem(System& system)
        {
            assert(!isRunning && "运行中的场景不允许调整系统！");
            assert(System::GetSystem(system.GetID()).has_value() && "场景中使用的系统必须是全局系统！");
            systems.emplace(&system);
        }
        void AddEntity(Entity entity)
        {
            entities.emplace(entity);
        }
        void RemoveSystem(System& system)
        {
            assert(!isRunning && "运行中的场景不允许调整系统！");
            assert(System::GetSystem(system.GetID()).has_value() && "场景中使用的系统必须是全局系统！");
            systems.erase(&system);
        }
        void RemoveEntity(const Entity entity)
        {
            entities.erase(entity);
        }

    private:
        friend class SceneWindow;

        inline static std::vector<std::unique_ptr<Scene>> allScenes = {};

        std::string name;
        std::unordered_set<System*> systems;
        std::unordered_set<Entity> entities;
        bool isRunning;
        // std::vector<Scene*> subScenes;
    };
}