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
    Gleam_MakeEngineStartEvent(InitWindowLibrary, std::numeric_limits<int>::lowest())
    {
        Window::Init();
    }
    Gleam_MakeEngineStopEvent(UnInitWindowLibrary, std::numeric_limits<int>::lowest())
    {
        Window::UnInit();
    }

    Gleam_AddRuntimeSystems(
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