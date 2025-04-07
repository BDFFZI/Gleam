#include <thread>

#include "GleamECS/Runtime/World/World.h"
#include "GleamEngine/Editor/Editor.h"
#include "GleamEngine/Editor/Profiler.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamReflection/RuntimeTests/Test.h"

Gleam_MakeArchetype(DataArchetype, CustomObject)

class MySystem : public System
{
    void Start() override
    {
        World::AddEntity(data);
    }
};
Gleam_MakeSystem(MySystem)
Gleam_AddEditorSystems(GlobalMySystem)

Gleam_Main