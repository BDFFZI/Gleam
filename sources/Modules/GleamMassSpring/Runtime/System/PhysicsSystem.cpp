#include "PhysicsSystem.h"

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"

namespace Gleam
{
    Entity PhysicsSystem::AddMassPoint(const float3 position, const float drag, const float mass)
    {
        const Entity entity = World::AddEntity(MassPointArchetype);
        World::SetComponents(entity, Particle{position, position, mass, drag});
        return entity;
    }
    Entity PhysicsSystem::AddSpring(const Entity particleA, const Entity particleB, const float elasticity)
    {
        Particle& pointA = World::GetComponent<Particle>(particleA);
        Particle& pointB = World::GetComponent<Particle>(particleB);
        float length = distance(pointA.position, pointB.position);

        Entity entity = World::AddEntity(SpringArchetype);
        World::SetComponents(entity, Spring{particleA, particleB, length, elasticity});
        return entity;
    }
    void PhysicsSystem::Update()
    {
        for (int i = 0; i < TimeSystem.GetFixedDeltaCount(); i++)
            SystemGroup::Update();
    }
}