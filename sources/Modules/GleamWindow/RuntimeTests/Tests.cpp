#include <iostream>
#include <gtest/gtest.h>

#include "GleamECS/Runtime/World.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamUtility/Runtime/Macro.h"
#include "GleamWindow/Runtime/System/CursorSystem.h"
#include "GleamWindow/Runtime/System/InputSystem.h"
#include "GleamWindow/Runtime/Window.h"

using namespace Gleam;

SystemEvent systemEvent = SystemEvent{"MySystem", GlobalPostUpdateSystem};
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
        if (GlobalInputSystem.GetKey(KeyCode::W))
            moveInput.y = 1;
        else if (GlobalInputSystem.GetKey(KeyCode::S))
            moveInput.y = -1;
        if (GlobalInputSystem.GetKey(KeyCode::A))
            moveInput.x = -1;
        else if (GlobalInputSystem.GetKey(KeyCode::D))
            moveInput.x = 1;
        if (any(moveInput))
            std::cout << "Move:" << to_string(moveInput) << '\n';
        //检查鼠标左键和鼠标位置输入
        if (GlobalInputSystem.GetMouseButton(MouseButton::Left))
            std::cout << "Fire:" << to_string(GlobalInputSystem.GetMousePosition()) << '\n';
        //检查鼠标位置增量
        if (GlobalInputSystem.GetKey(KeyCode::LeftShift))
        {
            std::cout << "LeftShift:" << to_string(GlobalInputSystem.GetMouseMoveDelta()) << '\n';
            windowPosition += int2(GlobalInputSystem.GetMouseMoveDelta().x, GlobalInputSystem.GetMouseMoveDelta().y);
            glfwSetWindowPos(Window::GetGlfwWindow(), windowPosition.x, windowPosition.y);
        }
        //检查输入区域功能
        if (GlobalInputSystem.GetMouseButtonDown(MouseButton::Right))
            GlobalInputSystem.SetFocusArea({Window::GetMousePosition(), float2(std::numeric_limits<float>::max())});
        //检查光标隐藏
        if (GlobalInputSystem.GetMouseButtonDown(MouseButton::Right))
        {
            GlobalCursorSystem.SetLockState(true);
            GlobalCursorSystem.SetVisible(false);
        }
        else if (GlobalInputSystem.GetMouseButtonUp(MouseButton::Right))
        {
            GlobalCursorSystem.SetLockState(false);
            GlobalCursorSystem.SetVisible(true);
        }
        //检查窗口大小修改
        if (GlobalInputSystem.GetKeyDown(KeyCode::Minus))
        {
            resolution /= 2;
            Window::SetResolution(static_cast<int2>(resolution));
        }
        else if (GlobalInputSystem.GetKeyDown(KeyCode::Equals))
        {
            resolution *= 2;
            Window::SetResolution(static_cast<int2>(resolution));
        }
        //检查全屏功能
        if (GlobalInputSystem.GetKeyDown(KeyCode::F11))
            Window::SetFullScreen(!Window::GetFullScreen());
    };

    World::AddSystem(systemEvent);
}

Gleam_Main