#include "Physics.h"

#include "Entity/Archetype.h"
#include "GleamECS/Runtime/World.h"

namespace Gleam
{
    Entity Physics::AddParticle(const float3 position, const float drag, const float mass)
    {
        const Entity entity = World::AddEntity(ParticleArchetype);
        World::GetEntityAllocator().SetComponents(entity, Particle{position, position, mass, drag});
        return entity;
    }
    Entity Physics::AddSpring(const Entity particleA, const Entity particleB, const float elasticity)
    {
        Particle& pointA = World::GetEntityAllocator().GetComponent<Particle>(particleA);
        Particle& pointB = World::GetEntityAllocator().GetComponent<Particle>(particleB);
        float length = distance(pointA.position, pointB.position);

        Entity entity = World::AddEntity(SpringArchetype);
        World::GetEntityAllocator().SetComponents(entity, Spring{particleA, particleB, length, elasticity});
        return entity;
    }
}