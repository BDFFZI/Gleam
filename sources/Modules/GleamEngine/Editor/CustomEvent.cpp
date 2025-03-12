#include "CustomEvent.h"

#include "Editor.h"
#include "GleamEngine/Runtime/Engine.h"
#include "System/ProfilerWindow.h"

namespace Gleam
{
    void ReplaceRuntimeSystem()
    {
        for (auto system : Engine::RuntimeSystems())
            World::RemoveSystem(system);
        World::FlushSystemQueue();
        for (auto system : Editor::EditorSystems())
            World::AddSystem(system);
    }
    void PlayOrStopEngine()
    {
        static bool lastIsPlaying = false;
        if (lastIsPlaying != Editor::IsPlaying())
        {
            if (Editor::IsPlaying())
            {
                for (auto system : Engine::RuntimeSystems())
                    World::AddSystem(system);
            }
            else
            {
                World::Clear();
                for (auto system : Editor::EditorSystems())
                    World::AddSystem(system);
            }
        }

        lastIsPlaying = Editor::IsPlaying();
    }
    void FetchProfile()
    {
        GlobalProfilerWindow.SetProfile(Profiler::FlushProfile());
    }
}