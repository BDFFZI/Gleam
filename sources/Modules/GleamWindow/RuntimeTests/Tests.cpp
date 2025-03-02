#include <iostream>
#include <gtest/gtest.h>

#include "GleamECS/Runtime/World.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamUtility/Runtime/Macro.h"
#include "GleamWindow/Runtime/System/CursorSystem.h"
#include "GleamWindow/Runtime/System/InputSystem.h"
#include "GleamWindow/Runtime/Window.h"

using namespace Gleam;

SystemEvent systemEvent = SystemEvent{"MySystem", PostUpdateSystem};
int2 windowPosition;
float2 resolution;

Gleam_MakeInitEvent()
{
    systemEvent.OnStart() = [&]
    {
        windowPosition = Window::GetWindowPosition();
        resolution = static_cast<float2>(Window::GetResolution());
    };
    systemEvent.OnUpdate() = [&]
    {
        //检查WASD输入
        float2 moveInput = 0;
        if (InputSystem.GetKey(KeyCode::W))
            moveInput.y = 1;
        else if (InputSystem.GetKey(KeyCode::S))
            moveInput.y = -1;
        if (InputSystem.GetKey(KeyCode::A))
            moveInput.x = -1;
        else if (InputSystem.GetKey(KeyCode::D))
            moveInput.x = 1;
        if (any(moveInput))
            std::cout << "Move:" << to_string(moveInput) << '\n';
        //检查鼠标左键和鼠标位置输入
        if (InputSystem.GetMouseButton(MouseButton::Left))
            std::cout << "Fire:" << to_string(InputSystem.GetMousePosition()) << '\n';
        //检查鼠标位置增量
        if (InputSystem.GetKey(KeyCode::LeftShift))
        {
            std::cout << "LeftShift:" << to_string(InputSystem.GetMouseMoveDelta()) << '\n';
            windowPosition += int2(InputSystem.GetMouseMoveDelta().x, InputSystem.GetMouseMoveDelta().y);
            glfwSetWindowPos(Window::GetGlfwWindow(), windowPosition.x, windowPosition.y);
        }
        //检查输入区域功能
        if (InputSystem.GetMouseButtonDown(MouseButton::Right))
            InputSystem.SetFocusArea({Window::GetMousePosition(), float2(std::numeric_limits<float>::max())});
        //检查光标隐藏
        if (InputSystem.GetMouseButtonDown(MouseButton::Right))
        {
            CursorSystem.SetLockState(true);
            CursorSystem.SetVisible(false);
        }
        else if (InputSystem.GetMouseButtonUp(MouseButton::Right))
        {
            CursorSystem.SetLockState(false);
            CursorSystem.SetVisible(true);
        }
        //检查窗口大小修改
        if (InputSystem.GetKeyDown(KeyCode::Minus))
        {
            resolution /= 2;
            Window::SetResolution(static_cast<int2>(resolution));
        }
        else if (InputSystem.GetKeyDown(KeyCode::Equals))
        {
            resolution *= 2;
            Window::SetResolution(static_cast<int2>(resolution));
        }
        //检查全屏功能
        if (InputSystem.GetKeyDown(KeyCode::F11))
            Window::SetFullScreen(!Window::GetFullScreen());
    };

    World::AddSystem(systemEvent);
}

Gleam_Main