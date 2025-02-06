#pragma once
#include "LightUI/Runtime/__Init__.h"

#include "InspectorWindow.h"
#include "HierarchyWindow.h"
#include "EditorUI/EditorUIUtility.h"

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

    inline void InspectorGUI_Entity(Entity* entity)
    {
        if (World::HasEntity(*entity))
            EditorUIUtility::DrawEntity(*entity);
    }
    Light_MakeInspectorGUI(Entity, InspectorGUI_Entity)
}