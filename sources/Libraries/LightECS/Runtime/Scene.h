#pragma once
#include <vector>

#include "Entity.h"
#include "System.h"

namespace Light
{
    struct Scene
    {
        std::vector<Entity> entities;
        std::vector<System*> systems;
    };    
}