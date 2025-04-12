#pragma once
#include "Cursor.h"
#include "Window.h"
#include "System/InputSystem.h"

namespace Gleam
{
    Gleam_MakeEngineStartEvent(InitWindowLibrary, std::numeric_limits<int>::lowest())
    {
        Window::Init();
    }
    Gleam_MakeEngineStopEvent(UnInitWindowLibrary, std::numeric_limits<int>::max())
    {
        Window::UnInit();
    }
    Gleam_MakeEngineUpdateEvent(UpdateWindowLibrary, std::numeric_limits<int>::max())
    {
        Window::Update();
        Cursor::Update();
        if (glfwWindowShouldClose(Window::GetGlfwWindow()))
            Engine::Stop();
    }
}