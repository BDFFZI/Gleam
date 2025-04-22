#include "GameRenderingSystem.h"

#include "GleamECS/Runtime/View/View.h"
#include "GleamMassSpring/Runtime/Entity/Spring.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"

void GameRenderingSystem::Start()
{
    using namespace Gleam;

    pointsRenderer = World::AddSceneEntity(PointRendererArchetype);
    linesRenderer = World::AddSceneEntity(LineRendererArchetype);
}
void GameRenderingSystem::Update()
{
    using namespace Gleam;

    particles.clear();
    World::GetView<Particle>().Each([this](Particle& particle)
    {
        particles.push_back({particle.position});
    });
    springs.clear();
    World::GetView<Spring>().Each([this](Spring& springPhysics)
    {
        Particle& particleA = World::GetEntityAllocator().GetComponent<Particle>(springPhysics.particleA);
        Particle& particleB = World::GetEntityAllocator().GetComponent<Particle>(springPhysics.particleB);
        springs.emplace_back(particleA.position, particleB.position);
    });

    World::GetEntityAllocator().GetComponent<PointsMesh>(pointsRenderer).points = particles;
    World::GetEntityAllocator().GetComponent<LinesMesh>(linesRenderer).lines = springs;
}