#pragma once

#include "Editor.h"
#include "GleamECS/Runtime/Scene.h"
#include "System/InspectorWindow.h"
#include "System/HierarchyWindow.h"
#include "System/ProfilerWindow.h"
#include "CustomUI.h"

namespace Gleam
{
    Gleam_AddStartEvent(ReplaceRuntimeSystem, 0)
    {
        for (auto system : Engine::RuntimeSystems())
            World::RemoveSystem(system);
        World::FlushSystemQueue();
        for (auto system : Editor::EditorSystems())
            World::AddSystem(system);
    }

    Gleam_AddUpdateEvent(PlayOrStop, 0)
    {
        static bool lastIsPlaying = false;
        if (lastIsPlaying != Editor::IsPlaying())
        {
            if (Editor::IsPlaying())
            {
                for (auto system : Engine::RuntimeSystems())
                    World::AddSystem(system);
                for (Scene& scene : Scene::GetAllScenes())
                    scene.Start();
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

    Gleam_AddUpdateEvent(FetchProfile, 1)
    {
        GlobalProfilerWindow.SetProfile(Profiler::FlushProfile());
    }

    Gleam_AddEditorSystems(
        GlobalEditorUISystem,
        GlobalHierarchyWindow,
        GlobalInspectorWindow,
        GlobalProfilerWindow,
    )

    Gleam_MakeInspectorUI(Entity, InspectorUI_Entity)
    Gleam_MakeInspectorUI(LocalTransform, InspectorUI_LocalTransform)
}