#pragma once
#include "UI.h"
#include "UISystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_MakeEngineStartEvent(InitUILibrary, InitGraphicsLibraryOrder+1)
    {
        UI::Init();
    }

    Gleam_MakeEngineStopEvent(UnInitUILibrary, UnInitGraphicsLibraryOrder-1)
    {
        UI::UnInit();
    }

    Gleam_AddRuntimeSystems(
        GlobalUISystem
    )

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(
        GlobalUISystem
    );
#endif
}