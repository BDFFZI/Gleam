#include <iostream>
#include <gtest/gtest.h>

#include "GleamECS/Runtime/World.h"
#include "GleamECS/Runtime/System/System.h"
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamWindow/Runtime/Cursor.h"
#include "GleamWindow/Runtime/Window.h"
#include "GleamWindow/Runtime/System/InputSystem.h"


using namespace Gleam;

class InputTest : public System<>
{
    int2 windowPosition = {};
    float2 resolution = {};

    void Start() override
    {
        windowPosition = Window::GetWindowPosition();
        resolution = static_cast<float2>(Window::GetResolution());
    }
    void Update() override
    {
        //检查WASD输入
        float2 moveInput = 0;
        if (GlobalInputSystem->GetKey(KeyCode::W))
            moveInput.y = 1;
        else if (GlobalInputSystem->GetKey(KeyCode::S))
            moveInput.y = -1;
        if (GlobalInputSystem->GetKey(KeyCode::A))
            moveInput.x = -1;
        else if (GlobalInputSystem->GetKey(KeyCode::D))
            moveInput.x = 1;
        if (any(moveInput))
            std::cout << "Move:" << to_string(moveInput) << '\n';
        //检查鼠标左键和鼠标位置输入
        if (GlobalInputSystem->GetMouseButton(MouseButton::Left))
            std::cout << "Fire:" << to_string(GlobalInputSystem->GetMousePosition()) << '\n';
        //检查鼠标位置增量
        if (GlobalInputSystem->GetKey(KeyCode::LeftShift))
        {
            std::cout << "LeftShift:" << to_string(GlobalInputSystem->GetMouseMoveDelta()) << '\n';
            windowPosition += int2(GlobalInputSystem->GetMouseMoveDelta().x, GlobalInputSystem->GetMouseMoveDelta().y);
            glfwSetWindowPos(Window::GetGlfwWindow(), windowPosition.x, windowPosition.y);
        }
        //检查输入区域功能
        if (GlobalInputSystem->GetMouseButtonDown(MouseButton::Right))
            GlobalInputSystem->SetFocusArea({Window::GetMousePosition(), float2(std::numeric_limits<float>::max())});
        //检查光标隐藏
        if (GlobalInputSystem->GetMouseButtonDown(MouseButton::Right))
        {
            Cursor::SetLockState(true);
            Cursor::SetVisible(false);
        }
        else if (GlobalInputSystem->GetMouseButtonUp(MouseButton::Right))
        {
            Cursor::SetLockState(false);
            Cursor::SetVisible(true);
        }
        //检查窗口大小修改
        if (GlobalInputSystem->GetKeyDown(KeyCode::Minus))
        {
            resolution /= 2;
            Window::SetResolution(static_cast<int2>(resolution));
        }
        else if (GlobalInputSystem->GetKeyDown(KeyCode::Equals))
        {
            resolution *= 2;
            Window::SetResolution(static_cast<int2>(resolution));
        }
        //检查全屏功能
        if (GlobalInputSystem->GetKeyDown(KeyCode::F11))
            Window::SetFullScreen(!Window::GetFullScreen());
    }
};
Gleam_MakeRuntimeSystem(InputTest)

Gleam_Main