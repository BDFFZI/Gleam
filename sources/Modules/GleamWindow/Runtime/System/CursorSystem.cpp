#include "CursorSystem.h"

#include "GleamWindow/Runtime/Window.h"

namespace Gleam
{
    bool CursorSystem::GetLockState() const
    {
        return isLocking;
    }
    float2 CursorSystem::GetLockPosition() const
    {
        return lockingPos;
    }
    void CursorSystem::SetLockState(const bool state)
    {
        if (state)
        {
            isLocking = true;
            lockingPos = Window::GetMousePosition();
        }
        else
            isLocking = false;
    }
    void CursorSystem::SetVisible(const bool state)
    {
        if (state)
            glfwSetInputMode(Window::GetGlfwWindow(),GLFW_CURSOR,GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(Window::GetGlfwWindow(),GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
    }
    void CursorSystem::Update()
    {
        if (isLocking)
            Window::SetMousePosition(lockingPos);
    }
}