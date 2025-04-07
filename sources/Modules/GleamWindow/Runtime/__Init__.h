#pragma once
#include "Library/Cursor.h"
#include "Library/Window.h"
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
    Gleam_MakeEngineUpdateEvent(UpdateWindowLibrary, std::numeric_limits<int>::max())
    {
        Cursor::Update();
        Window::Update();
        if (glfwWindowShouldClose(Window::GetGlfwWindow()))
            Engine::Stop();
    }

    Gleam_AddRuntimeSystems(
        InputSystem
    )

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(
        InputSystem
    );
#endif
}