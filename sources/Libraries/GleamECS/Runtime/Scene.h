#pragma once
#include <unordered_set>
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

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
        static std::optional<std::reference_wrapper<Scene>> GetScene(std::string_view name);
        static std::optional<std::reference_wrapper<Scene>> GetScene(System& system);
        static std::optional<std::reference_wrapper<Scene>> GetScene(Entity entity);

        /**
         * 创建一个空Scene
         * @param name
         * @param isRunning 
         * @return 
         */
        static Scene& Create(std::string_view name = "", bool isRunning = false);
        /**
         * 移除Scene并销毁或释放其托管的相关资源
         *
         * 释放即让场景放弃其对托管资源的所有权，原本其托管的所有System和Entity将完全交由World管理。
         * 因为除了Scene，World也有回收Entity和System的权力，当由World回收时，Scene应当释放所有权。
         * @param scene
         * @param onlyRelease 
         */
        static void Destroy(Scene& scene, bool onlyRelease = false);

        Scene() = default;
        Scene(Scene&) = delete;
        Scene& operator=(Scene&) = delete;

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

        void AddSystem(System& system);
        void RemoveSystem(System& system);
        void AddEntity(Entity entity);
        void RemoveEntity(Entity entity);
        void AddSubScene(Scene& scene)
        {
            subScenes.insert(&scene);
        }
        void RemoveSubScene(Scene& scene)
        {
            subScenes.erase(&scene);
        }

    private:
        friend class SceneAsset;
        inline static std::vector<std::unique_ptr<Scene>> allScenes = {};
        inline static std::unordered_map<System*, Scene*> systemToWorld = {};
        inline static std::unordered_map<Entity, Scene*> entityToWorld = {};

        std::string name;
        std::unordered_set<System*> systems;
        std::unordered_set<Entity> entities;
        std::unordered_set<Scene*> subScenes;
        bool isRunning = false;
    };
}