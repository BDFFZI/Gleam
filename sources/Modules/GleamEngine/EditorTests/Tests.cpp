#include <thread>

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Editor/Editor.h"
#include "GleamEngine/Editor/Profiler.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamReflection/RuntimeTests/Test.h"

using namespace Gleam;

Gleam_MakeArchetype(DataArchetype, CustomData)


class MySystem : public System
{
    void Start() override
    {
        Entity entity = World::AddEntity(DataArchetype);
        World::SetComponents(entity, data);
    }
    void Update() override
    {
        {
            Gleam_ProfilerSample("Test 1");
            {
                {
                    Gleam_ProfilerSample("Test A");
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        {
            Gleam_ProfilerSample("Test 2");
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }
    }
};
Gleam_MakeGlobalSystem(MySystem)
Gleam_AddEditorSystems(GlobalMySystem)

Gleam_Main