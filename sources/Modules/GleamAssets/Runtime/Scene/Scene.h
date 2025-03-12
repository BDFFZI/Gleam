#pragma once
#include <unordered_set>
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
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

        /**
         * 创建一个空Scene
         * @param name 
         * @return 
         */
        static Scene& Create(std::string_view name);
        /**
         * 销毁Scene及其托管的相关资源
         * @param name 
         */
        static void Destroy(std::string_view name);
        static void Destroy(Scene& scene);
        /**
         * 销毁或释放所有Scene，并清理Scene存储器。
         *
         * 通常用于在World.Clear()前调用，以便转交资源所有权给World，并清理Scene存储器
         */
        static void Clear(bool release = false);

        static std::optional<std::reference_wrapper<Scene>> GetScene(std::string_view name);

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

        /**
         * 将Systems添加到世界中以接收更新事件
         */
        void Start();
        /**
         * 将Systems移除出世界中以停止更新事件
         */
        void Stop();
        /**
         * 释放所有权，将所有System和Entity完全交由World管理。
         *
         * 除了Scene，World也有回收Entity和System的权力，当由World回收时，Scene应当释放所有权。
         */
        void Release();
        /**
         * 销毁Scene托管的所有资源。
         */
        void Reset();

        void AddSystem(System& system);
        void RemoveSystem(System& system);
        void AddEntity(Entity entity);
        void RemoveEntity(Entity entity);

    private:
        friend class SceneAsset;
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