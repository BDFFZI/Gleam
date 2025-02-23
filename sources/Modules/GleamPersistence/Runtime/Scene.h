#pragma once
#include <vector>

#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
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