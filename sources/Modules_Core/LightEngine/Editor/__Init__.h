#pragma once
#include "EditorUI/HierarchyWindow.h"
#include "EditorUI/InspectorWindow.h"

namespace Light
{
    Light_AddSystems(EditorUISystem)
    Light_AddSystems(HierarchyWindow)
    Light_AddSystems(InspectorWindow)
}

#include "LightUI/Runtime/__Init__.h"