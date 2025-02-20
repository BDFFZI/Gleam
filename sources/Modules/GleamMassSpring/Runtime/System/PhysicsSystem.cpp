#include "PhysicsSystem.h"

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"

namespace Gleam
{
    Entity PhysicsSystem::AddMassPoint(const float3 position, float drag, float mass)
    {
        Point point = Point{position};
        MassPointLastState massPointLastState = {};
        massPointLastState.lastPosition = position;
        MassPointPhysics massPointPhysics = {};
        massPointPhysics.drag = drag;
        massPointPhysics.mass = mass;

        const Entity entity = World::AddEntity(MassPointArchetype);
        World::SetComponents(entity, point, massPointLastState, massPointPhysics);
        return entity;
    }
    Entity PhysicsSystem::AddSpring(const Entity massPointA, const Entity massPointB, const float elasticity)
    {
        Point pointA = World::GetComponent<Point>(massPointA);
        Point pointB = World::GetComponent<Point>(massPointB);
        float length = distance(pointA.position, pointB.position);

        Entity entity = World::AddEntity(SpringArchetype);
        World::SetComponents(entity, SpringPhysics{massPointA, massPointB, length, elasticity});
        return entity;
    }
    void PhysicsSystem::Update()
    {
        for (int i = 0; i < TimeSystem.GetFixedDeltaCount(); i++)
            SystemGroup::Update();
    }
}