#include "WindowSystem.h"

#include <GLFW/glfw3.h>

#include "LightEngine/Runtime/Engine.h"
#include "LightWindow/Runtime/Window.h"

namespace Light
{
    void WindowSystem::Update()
    {
        Window::Update();

        SystemGroup::Update();

        if (glfwWindowShouldClose(Window::GetGlfwWindow()))
            Engine::Stop();
    }
}