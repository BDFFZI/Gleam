#include <gtest/gtest.h>

#include "LightEngine/Runtime/Engine.h"
#include "LightReflection/RuntimeTests/Test.h"

using namespace Light;

Light_MakeArchetype(DataArchetype, TestData)

int main()
{
    Entity entity = Awake->AddEntity(DataArchetype);
    World::SetComponents(entity, data);

    Engine::Start();
    return 0;
}