#include "FixedPointSystem.h"

#include "LogicSystem.h"
#include "Physics/PhysicsComponent.hpp"
#include "Public/Component.hpp"

using namespace Light;

void FixedPointSystem::Update()
{
    if (LogicSystem.GetFixedPoint() != Entity::Null)
    {
        Point* point;
        MassPointPhysics* massPointPhysics;
        World::GetComponents(LogicSystem.GetFixedPoint(), &point, &massPointPhysics);
        massPointPhysics->force = 0;
        massPointPhysics->velocity = 0;
    }
}
