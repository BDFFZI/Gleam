#pragma once
#include <unordered_set>
#include <vector>

#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    /**
     * 场景是一种能将世界中的实体和系统分组并持久化的容器。
     * 其能将直接或间接包含的实体和系统存储到磁盘中，并在需要时再加载回内存，且支持跨场景加载。
     */
    class Scene
    {
    public:
        static Scene& Create(std::string_view name);
        static Scene& Load(std::string_view name);
        static void Save(Scene& scene);

        const std::string& GetName() const;
        void AddEntity(Entity entity);
        void AddSystem(System& system);

    private:
        friend class SceneWindow;

        inline static std::unordered_map<std::string_view, Scene> allScenes = {};

        std::string name;
        std::unordered_set<Entity> entities;
        std::unordered_set<System*> systems;
        // std::vector<Scene*> subScenes;
    };
}