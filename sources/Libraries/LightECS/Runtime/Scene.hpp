#pragma once
#include <vector>

#include "Entity.hpp"
#include "System.h"

namespace Light
{
    struct Scene
    {
        std::vector<Entity> entities;
        std::vector<System*> systems;
    };    
}