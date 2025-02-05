#include <iostream>

#include "LightECS/Runtime/System.h"
#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/System/TimeSystem.h"
#include "LightMassSpring/Runtime/Entity/Archetype.h"

using namespace Light;

class TestSystem : public System
{
    void Start() override
    {
        Awake->AddEntity(MassPointRendererArchetype);
        Awake->AddEntity(SpringRendererArchetype);

        Entity pointA = Awake->AddEntity(MassPointArchetype);
        MassPointPhysics massPointPhysics = {};
        massPointPhysics.drag = 1;
        World::SetComponents(pointA, massPointPhysics);

        Entity pointB = Awake->AddEntity(MassPointArchetype);
        Point point = {{20,0,0}};
        World::SetComponents(pointB, point);

        Entity spring = Awake->AddEntity(SpringArchetype);
        SpringPhysics springPhysics = {};
        springPhysics.pointA = pointA;
        springPhysics.pointB = pointB;
        World::SetComponents(spring, springPhysics);
    }
};
Light_MakeSystem(TestSystem)
Light_AddSystems(TestSystem)