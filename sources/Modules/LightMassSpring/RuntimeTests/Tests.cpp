#include <iostream>

#include "LightECS/Runtime/System.h"
#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/System/TimeSystem.h"
#include "LightMassSpring/Runtime/Entity/Archetype.h"

using namespace Light;

class TestSystem : public System
{
    void Start() override
    {
        World::AddEntity(MassPointRendererArchetype);
        World::AddEntity(SpringRendererArchetype);

        Entity pointA = World::AddEntity(MassPointArchetype);
        MassPointPhysics massPointPhysics = {};
        massPointPhysics.drag = 1;
        World::SetComponents(pointA, massPointPhysics);

        Entity pointB = World::AddEntity(MassPointArchetype);
        Point point = {{20,0,0}};
        World::SetComponents(pointB, point);

        Entity spring = World::AddEntity(SpringArchetype);
        SpringPhysics springPhysics = {};
        springPhysics.pointA = pointA;
        springPhysics.pointB = pointB;
        World::SetComponents(spring, springPhysics);
    }
};
Light_MakeGlobalSystem(TestSystem)
Light_AddEditorSystems(TestSystem)