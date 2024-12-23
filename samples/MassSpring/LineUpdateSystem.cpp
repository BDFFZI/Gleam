#include "LineUpdateSystem.h"
#include "LightEngine/Runtime/ECS/View.hpp"
#include "Physics/PhysicsComponent.hpp"

using namespace Light;

void LineUpdateSystem::Update()
{
    View<Line, SpringPhysics>::Each([](Line& line, SpringPhysics& springPhysics)
    {
        Point pointA;
        MassPointPhysics massPointPhysicsA;
        springPhysics.pointA.Get(&pointA, &massPointPhysicsA);
        Point pointB;
        MassPointPhysics massPointPhysicsB;
        springPhysics.pointB.Get(&pointB, &massPointPhysicsB);

        line.positionA = pointA.position;
        line.positionB = pointB.position;
    });
}
