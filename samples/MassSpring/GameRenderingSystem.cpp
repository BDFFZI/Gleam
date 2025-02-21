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
    View<Point, Particle>::Each([this](Point& point, Particle& pointPhysics)
    {
        particles.push_back(point);
    });
    springs.clear();
    View<Spring>::Each([this](Spring& springPhysics)
    {
        Point pointA;
        Particle particleA;
        World::GetComponents(springPhysics.particleA, pointA, particleA);
        Point pointB;
        Particle particleB;
        World::GetComponents(springPhysics.particleB, pointB, particleB);

        springs.emplace_back(pointA.position, pointB.position);
    });

    World::GetComponent<PointsMesh>(pointsRenderer).points = particles;
    World::GetComponent<LinesMesh>(linesRenderer).lines = springs;
}