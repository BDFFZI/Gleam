#pragma once

#include "Editor.h"
#include "EditorUI/EditorUISerializer.h"
#include "System/InspectorWindow.h"
#include "System/HierarchyWindow.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamUI/Runtime/UI.h"
#include "System/ProfilerWindow.h"

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
                {
                    World::AddSystem(system);
                }
            }
            else
            {
                World::Clear();
                for (auto system : Editor::EditorSystems())
                {
                    World::AddSystem(system);
                }
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

    inline void InspectorUI_LocalTransform(LocalTransform& localTransform)
    {
        EditorUISerializer serializer = {"LocalTransform"};
        serializer.TransferField("position", localTransform.position);
        float3 eulerAngles = localTransform.rotation.ToEulerAngles();
        serializer.TransferField("rotation", eulerAngles);
        localTransform.rotation = Quaternion::Euler(eulerAngles);
        serializer.TransferField("scale", localTransform.scale);
    }
    Gleam_MakeInspectorUI(LocalTransform, InspectorUI_LocalTransform)

    inline void InspectorUI_Entity(const Entity entity)
    {
        if (World::HasEntity(entity))
        {
            EditorUI::DrawEntity(
                entity,
                InspectorWindow::UseDebugGUI() ? CustomUI{} : InspectorWindow::GetCustomUI()
            );
        }
    }
    Gleam_MakeInspectorUI(Entity, InspectorUI_Entity)

    inline void InspectorUI_System(System& system)
    {
        ImGui::Text("System");
    }
    Gleam_MakeInspectorUI(System, InspectorUI_System)
}