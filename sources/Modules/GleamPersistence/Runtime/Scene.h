#pragma once
#include <vector>

#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    /**
     * 场景一种将世界中的实体和系统分组的容器，此外还支持持久化功能。
     * 能将直接或间接包含的实体和系统存储到磁盘中，并在需要时再加载回内存，且支持跨场景加载。
     */
    class Scene
    {
    public:
        std::string& Name() { return name; }
        std::vector<Entity>& Entities() { return entities; }
        std::vector<System*>& Systems() { return systems; }
        std::vector<Scene*>& SubScenes() { return subScenes; }

    private:
        std::string name;
        std::vector<Entity> entities;
        std::vector<System*> systems;
        std::vector<Scene*> subScenes;
    };
}