#pragma once
#include "UI.h"
#include "UISystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_MakeInitEvent()
    {
        Engine::AddStartEvent([]
        {
            UI::Init();
        }, InitGraphicsLibraryOrder + 1);
        Engine::AddStopEvent([]
        {
            UI::UnInit();
        }, UnInitGraphicsLibraryOrder - 1);

        Engine::AddRuntimeSystems<UISystem>();
#ifdef GleamEngineEditor
        Editor::AddEditorSystems<UISystem>();
#endif
    }
}