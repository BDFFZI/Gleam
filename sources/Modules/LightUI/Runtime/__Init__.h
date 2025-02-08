#pragma once
#include "LightPresentation/Runtime/__Init__.h"

#include "UISystem.h"

#ifdef Light_Editor
#include "LightEngine/Editor/Editor.h"
#endif

namespace Light
{
    Light_AddSystems(
        UISystem
    )

#ifdef Light_Editor
    Light_AddEditorSystems(
        UISystem
    );
#endif
}