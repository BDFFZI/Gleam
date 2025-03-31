#include "GameRenderingSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"

void GameRenderingSystem::Start()
{
    using namespace Gleam;

    pointsRenderer = World::AddEntity(PointRendererArchetype);
    linesRenderer = World::AddEntity(LineRendererArchetype);
}
void GameRenderingSystem::Update()
{
    using namespace Gleam;

    particles.clear();
    View<Particle>::Each([this](Particle& particle)
    {
        particles.push_back({particle.position});
    });
    springs.clear();
    View<Spring>::Each([this](Spring& springPhysics)
    {
        Particle& particleA = World::GetComponent<Particle>(springPhysics.particleA);
        Particle& particleB = World::GetComponent<Particle>(springPhysics.particleB);
        springs.emplace_back(particleA.position, particleB.position);
    });

    World::GetComponent<PointsMesh>(pointsRenderer).points = particles;
    World::GetComponent<LinesMesh>(linesRenderer).lines = springs;
}