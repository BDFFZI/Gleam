#pragma once
#include "PhysicsSystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class CollisionSystem : public System
    {
    public:
        CollisionSystem(): System(&PhysicsSystem, MiddleOrder, RightOrder)
        {
        }
    };
    inline CollisionSystem CollisionSystem = {};
}
