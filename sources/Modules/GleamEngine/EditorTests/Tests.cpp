#include <thread>

#include "GleamEngine/Editor/Editor.h"
#include "GleamEngine/Editor/Profiler.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamReflection/RuntimeTests/Test.h"

Gleam_MakeArchetype(DataArchetype, CustomObject)

class MySystem : public System<>
{
    void Start() override
    {
        World::GetEntityAllocator().AddEntity(data);
    }
};
Gleam_MakeEditorSystem(MySystem)

Gleam_Main