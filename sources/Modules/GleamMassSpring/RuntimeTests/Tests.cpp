#include "GleamECS/Runtime/System.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

using namespace Gleam;

class TestSystem : public System
{
    void Start() override
    {
#ifdef GleamRenderingRuntime
        World::AddEntity(MassPointRendererArchetype);
        World::AddEntity(SpringRendererArchetype);
#endif

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
Gleam_MakeSystemInstance(TestSystem)
Gleam_AddSystems(TestSystem)

#ifdef GleamEngineEditor
Gleam_AddEditorSystems(TestSystem)
#endif