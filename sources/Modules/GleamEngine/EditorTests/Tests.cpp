#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamReflection/RuntimeTests/Test.h"

using namespace Gleam;

Gleam_MakeArchetype(DataArchetype, TestData)

Gleam_MakeInitEvent()
{
    Entity entity = World::AddEntity(DataArchetype);
    World::SetComponents(entity, data);
}

Gleam_Main