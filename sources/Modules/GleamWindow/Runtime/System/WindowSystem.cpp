#include "WindowSystem.h"

#include <GLFW/glfw3.h>

#include "GleamEngine/Runtime/Engine.h"
#include "GleamWindow/Runtime/Window.h"

namespace Gleam
{
    void WindowSystem::Update()
    {
        Window::Update();

        SystemGroup::Update();

        if (glfwWindowShouldClose(Window::GetGlfwWindow()))
            Engine::Stop();
    }
}