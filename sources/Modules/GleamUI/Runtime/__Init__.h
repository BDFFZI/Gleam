#pragma once
#include "UI.h"
#include "UISystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_AddStartEvent(InitUILibrary, InitGraphicsLibraryOrder+1)
    {
        UI::Init();
    }

    Gleam_AddStopEvent(UnInitUILibrary, UnInitGraphicsLibraryOrder-1)
    {
        UI::UnInit();
    }

    Gleam_AddSystems(
        UISystem
    )

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(
        UISystem
    );
#endif
}