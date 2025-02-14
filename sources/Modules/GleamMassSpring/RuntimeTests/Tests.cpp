#include "GleamECS/Runtime/System.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"

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

        Entity pointA = PhysicsSystem::AddMassPoint(0);
        MassPointPhysics massPointPhysics = {};
        massPointPhysics.drag = 1;
        World::SetComponents(pointA, massPointPhysics);

        Entity pointB = PhysicsSystem::AddMassPoint({10, 0, 0});

        PhysicsSystem::AddSpring(pointA, pointB);
    }
};
Gleam_MakeGlobalSystem(TestSystem)
Gleam_AddSystems(TestSystem)

#ifdef GleamEngineEditor
Gleam_AddEditorSystems(TestSystem)
#endif

Gleam_Main