#include "PhysicsSystem.h"

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"

namespace Gleam
{
    Entity PhysicsSystem::AddMassPoint(float3 position)
    {
        Point point = Point{position};
        MassPointPhysics massPointPhysics = {};
        massPointPhysics.lastPosition = position;

        const Entity entity = World::AddEntity(MassPointArchetype);
        World::SetComponents(entity, point, massPointPhysics);
        return entity;
    }
    Entity PhysicsSystem::AddSpring(const Entity massPointA, const Entity massPointB)
    {
        Point pointA = World::GetComponent<Point>(massPointA);
        Point pointB = World::GetComponent<Point>(massPointB);
        float length = distance(pointA.position, pointB.position);

        Entity entity = World::AddEntity(SpringArchetype);
        World::SetComponents(entity, SpringPhysics{massPointA, massPointB, length});
        return entity;
    }
    void PhysicsSystem::Update()
    {
        float currentTime = TimeSystem.GetTime();
        float deltaTime = currentTime - lastTime;
        while (deltaTime >= fixedDeltaTime)
        {
            SystemGroup::Update();

            deltaTime -= fixedDeltaTime;
            lastTime += fixedDeltaTime;
        }
    }
}