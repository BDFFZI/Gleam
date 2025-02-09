#pragma once
#include "UI.h"
#include "LightPresentation/Runtime/__Init__.h"

#include "UISystem.h"

#ifdef LightEngineEditor
#include "LightEngine/Editor/Editor.h"
#endif

namespace Light
{
    Light_AddStartEvent(InitUILibrary, InitGraphicsLibraryOrder+1)
    {
        UI::Init();
    }

    Light_AddStopEvent(UnInitUILibrary, UnInitGraphicsLibraryOrder-1)
    {
        UI::UnInit();
    }

    Light_AddSystems(
        UISystem
    )

#ifdef LightEngineEditor
    Light_AddEditorSystems(
        UISystem
    );
#endif
}