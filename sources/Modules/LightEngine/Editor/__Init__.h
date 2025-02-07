#pragma once

#include "Editor.h"
#include "LightUI/Runtime/__Init__.h"

#include "System/InspectorWindow.h"
#include "System/HierarchyWindow.h"
#include "LightEngine/Editor/EditorUI/EditorUI.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    Light_AddSystems(
        EditorUISystem,
        HierarchyWindow,
        InspectorWindow
    )

    inline void EditorMenu_Play()
    {
        Editor::IsPlaying() = !Editor::IsPlaying();
    }
    Light_MakeEditorMenu("Edit/Play | Stop", EditorMenu_Play)

    inline void InspectorGUI_LocalTransform(LocalTransform& localTransform)
    {
        ImGui::DragFloat3("position", &localTransform.position[0]);
        float3 eulerAngles = localTransform.rotation.ToEulerAngles();
        ImGui::DragFloat3("rotation", &eulerAngles[0]);
        localTransform.rotation = Quaternion::Euler(eulerAngles);
        ImGui::DragFloat3("scale", &localTransform.scale[0]);
    }
    Light_MakeInspectorGUI(LocalTransform, InspectorGUI_LocalTransform)

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
    Light_MakeInspectorGUI(Entity, InspectorGUI_Entity)

    inline void InspectorGUI_System(System& system)
    {
        ImGui::Text("System");
    }
    Light_MakeInspectorGUI(System, InspectorGUI_System)
}