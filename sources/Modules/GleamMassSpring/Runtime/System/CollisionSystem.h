#pragma once
#include "PhysicsSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class CollisionSystem : public System
    {
    public:
        CollisionSystem(): System(PhysicsSystem, MiddleOrder, RightOrder)
        {
        }
    };
}