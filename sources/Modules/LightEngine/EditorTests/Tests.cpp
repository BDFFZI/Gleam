#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightReflection/RuntimeTests/Test.h"

using namespace Light;

Light_MakeArchetype(DataArchetype, TestData)

Light_MakeInitEvent()
{
    Entity entity = World::AddEntity(DataArchetype);
    World::SetComponents(entity, data);
}