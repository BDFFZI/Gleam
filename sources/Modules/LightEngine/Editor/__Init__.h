#pragma once

#include "LightUI/Runtime/__Init__.h"

#include "InspectorWindow.h"
#include "HierarchyWindow.h"
#include "EditorUI/EditorUI.h"
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
        Engine::isUpdating = !Engine::isUpdating;
    }
    Light_MakeEditorMenu("Edit/Play | Stop", EditorMenu_Play)

    inline void InspectorGUI_LocalTransform(LocalTransform* localTransform)
    {
        ImGui::DragFloat3("position", &localTransform->position[0]);
        float3 eulerAngles = localTransform->rotation.ToEulerAngles();
        ImGui::DragFloat3("rotation", &eulerAngles[0]);
        localTransform->rotation = Quaternion::Euler(eulerAngles);
        ImGui::DragFloat3("scale", &localTransform->scale[0]);
    }
    Light_MakeInspectorGUI(LocalTransform, InspectorGUI_LocalTransform)

    inline void InspectorGUI_Entity(Entity* entity)
    {
        if (World::HasEntity(*entity))
        {
            EditorUI::DrawEntity(
                *entity,
                InspectorWindow::UseDebugGUI() ? CustomGUI{} : InspectorWindow::GetCustomGUI()
            );
        }
    }
    Light_MakeInspectorGUI(Entity, InspectorGUI_Entity)
}