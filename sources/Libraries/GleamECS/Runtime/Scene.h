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

    //Entity包装器，使Entity可以被资源化
    struct EntityAsset
    {
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
        static AssetBundle& ToAssetBundle(const Scene& scene)
        {
            AssetBundle& assetBundle = AssetBundle::Create();

            //保存场景和系统信息
            SceneAsset sceneAsset;
            sceneAsset.name = scene.name;
            for (System* system : scene.systems)
            {
                sceneAsset.systems.push_back(system->GetID());
            }

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
            std::string_view name;
            std::vector<System*> systems;

            //读取实体信息
            std::vector<Entity> entities;
            for (std::size_t i = 1; i < assetCount; ++i)
            {
                EntityAsset entityAsset = *static_cast<EntityAsset*>(assets[i].GetDataRef());
                entities.emplace_back(entityAsset.entity);
            }

            Scene& scene = Create(name);
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

        bool HasSystem(System* system) const
        {
            return systems.contains(system);
        }
        bool HasEntity(const Entity entity) const
        {
            return entities.contains(entity);
        }
        void AddSystem(System& system)
        {
            systems.emplace(&system);
        }
        void AddEntity(Entity entity)
        {
            entities.emplace(entity);
        }

    private:
        friend class SceneWindow;

        inline static std::vector<std::unique_ptr<Scene>> allScenes = {};

        std::string name;
        std::unordered_set<System*> systems;
        std::unordered_set<Entity> entities;
        // std::vector<Scene*> subScenes;
    };
}