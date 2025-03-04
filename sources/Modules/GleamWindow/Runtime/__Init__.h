#pragma once
#include "Window.h"
#include "System/CursorSystem.h"
#include "System/WindowSystem.h"
#include "System/InputSystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_AddStartEvent(InitWindowLibrary, 0)
    {
        Window::Init();
    }
    Gleam_AddStopEvent(UnInitWindowLibrary, 0)
    {
        Window::UnInit();
    }

    Gleam_AddSystems(
        GlobalCursorSystem,
        GlobalWindowSystem,
        GlobalInputSystem
    )

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(
        GlobalCursorSystem,
        GlobalWindowSystem,
    );
#endif
}