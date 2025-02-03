#include "Cursor.h"

namespace Light
{
    bool Cursor::GetLockState() const
    {
        return isLocking;
    }
    float2 Cursor::GetLockPosition() const
    {
        return lockingPos;
    }
    void Cursor::SetLockState(const bool state)
    {
        if (state)
        {
            isLocking = true;
            lockingPos = Window->GetMousePosition();
        }
        else
            isLocking = false;
    }
    void Cursor::SetVisible(const bool state)
    {
        if (state)
            glfwSetInputMode(Window->GetGlfwWindow(),GLFW_CURSOR,GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(Window->GetGlfwWindow(),GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
    }
    void Cursor::Update()
    {
        if (isLocking)
            Window->SetMousePosition(lockingPos);
    }
}