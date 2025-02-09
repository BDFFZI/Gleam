#pragma once
#include <vector>

#include "Entity.h"
#include "System.h"

namespace Light
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