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

    massPoints.clear();
    View<Point, MassPointPhysics>::Each([this](Point& point, MassPointPhysics& pointPhysics)
    {
        massPoints.push_back(point);
    });
    springs.clear();
    View<SpringPhysics>::Each([this](SpringPhysics& springPhysics)
    {
        Point pointA;
        MassPointPhysics massPointPhysicsA;
        World::GetComponents(springPhysics.pointA, pointA, massPointPhysicsA);
        Point pointB;
        MassPointPhysics massPointPhysicsB;
        World::GetComponents(springPhysics.pointB, pointB, massPointPhysicsB);

        springs.emplace_back(pointA.position, pointB.position);
    });

    World::GetComponent<PointsMesh>(pointsRenderer).points = massPoints;
    World::GetComponent<LinesMesh>(linesRenderer).lines = springs;
}