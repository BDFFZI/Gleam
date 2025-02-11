#pragma once
#include "PhysicsSystem.h"
#include "PositionSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class CollisionSystem : public System
    {
    public:
        CollisionSystem(): System(PositionSystem, OrderRelation::After)
        {
        }
    };
}