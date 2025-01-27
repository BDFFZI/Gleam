#pragma once
#include "InspectorWindow.h"
#include "HierarchyWindow.h"
#include "LightEngine/Runtime/Engine.h"

namespace Light
{
    Light_AddSystems(EditorUISystem)
    Light_AddSystems(HierarchyWindow)
    Light_AddSystems(InspectorWindow)
}

#include "LightUI/Runtime/__Init__.h"