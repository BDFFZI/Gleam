#pragma once

#include "Editor.h"
#include "EditorUI/EditorUISerializer.h"
#include "System/InspectorWindow.h"
#include "System/HierarchyWindow.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    Gleam_AddStartEvent(ReplaceRuntimeSystem, 0)
    {
        for (auto* system : Engine::RuntimeSystems())
            World::RemoveSystem(system);
        World::FlushSystemQueue();
        for (auto* system : Editor::EditorSystems())
            World::AddSystem(system);
    }

    Gleam_AddUpdateEvent(PlayOrStop, 0)
    {
        static bool lastIsPlaying = false;
        if (lastIsPlaying != Editor::IsPlaying())
        {
            if (Editor::IsPlaying())
            {
                for (auto* system : Engine::RuntimeSystems())
                {
                    World::AddSystem(system);
                }
            }
            else
            {
                World::Clear();
                for (auto* system : Editor::EditorSystems())
                {
                    World::AddSystem(system);
                }
            }
        }

        lastIsPlaying = Editor::IsPlaying();
    }

    Gleam_AddEditorSystems(
        EditorUISystem,
        HierarchyWindow,
        InspectorWindow,
    )

    inline void EditorMenu_Play()
    {
        Editor::IsPlaying() = !Editor::IsPlaying();
    }
    Gleam_MakeEditorMenu("Edit/Play | Pause", EditorMenu_Play)

    inline void InspectorGUI_LocalTransform(LocalTransform& localTransform)
    {
        EditorUISerializer serializer;
        serializer.TransferField("position", localTransform.position);
        float3 eulerAngles = localTransform.rotation.ToEulerAngles();
        serializer.TransferField("rotation", eulerAngles);
        localTransform.rotation = Quaternion::Euler(eulerAngles);
        serializer.TransferField("scale", localTransform.scale);
    }
    Gleam_MakeInspectorGUI(LocalTransform, InspectorGUI_LocalTransform)

    inline void InspectorGUI_Entity(const Entity entity)
    {
        if (World::HasEntity(entity))
        {
            EditorUI::DrawEntity(
                entity,
                InspectorWindow::UseDebugGUI() ? CustomGUI{} : InspectorWindow::GetCustomGUI()
            );
        }
    }
    Gleam_MakeInspectorGUI(Entity, InspectorGUI_Entity)

    inline void InspectorGUI_System(System& system)
    {
        ImGui::Text("System");
    }
    Gleam_MakeInspectorGUI(System, InspectorGUI_System)
}