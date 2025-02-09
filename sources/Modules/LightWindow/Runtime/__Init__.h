#pragma once
#include "Window.h"
#include "LightEngine/Runtime/__Init__.h"

#include "System/CursorSystem.h"
#include "System/WindowSystem.h"
#include "System/InputSystem.h"

#ifdef LightEngineEditor
#include "LightEngine/Editor/Editor.h"
#endif

namespace Light
{
    Light_AddStartEvent(InitWindowLibrary, 0)
    {
        Window::Init();
    }
    Light_AddStopEvent(UnInitWindowLibrary, 0)
    {
        Window::UnInit();
    }

    Light_AddSystems(
        CursorSystem,
        WindowSystem,
        InputSystem
    )

#ifdef LightEngineEditor
    Light_AddEditorSystems(
        CursorSystem,
        WindowSystem,
    );
#endif
}