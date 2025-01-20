#include "LineUpdateSystem.h"
#include "LightECS/Runtime/View.hpp"
#include "Physics/PhysicsComponent.hpp"

using namespace Light;

void LineUpdateSystem::Update()
{
    View<Segment, SpringPhysics>::Each([](Segment& line, SpringPhysics& springPhysics)
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
