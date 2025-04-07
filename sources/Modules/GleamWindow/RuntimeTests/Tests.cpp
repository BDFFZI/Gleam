#include <iostream>
#include <gtest/gtest.h>

#include "GleamECS/Runtime/World/World.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamUtility/Runtime/Macro.h"
#include "GleamWindow/Runtime/Library/Cursor.h"
#include "GleamWindow/Runtime/Library/Window.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

using namespace Gleam;

class InputTest : public SystemT<>
{
    InputSystem* inputSystem = nullptr;
    int2 windowPosition = {};
    float2 resolution = {};

    void Start() override
    {
        inputSystem = GetWorld().GetSystem<InputSystem>().lock().get();

        windowPosition = Window::GetWindowPosition();
        resolution = static_cast<float2>(Window::GetResolution());
    }
    void Update() override
    {
        Input& input = inputSystem->GetDefaultInput();

        //检查WASD输入
        float2 moveInput = 0;
        if (input.GetKey(KeyCode::W))
            moveInput.y = 1;
        else if (input.GetKey(KeyCode::S))
            moveInput.y = -1;
        if (input.GetKey(KeyCode::A))
            moveInput.x = -1;
        else if (input.GetKey(KeyCode::D))
            moveInput.x = 1;
        if (any(moveInput))
            std::cout << "Move:" << to_string(moveInput) << '\n';
        //检查鼠标左键和鼠标位置输入
        if (input.GetMouseButton(MouseButton::Left))
            std::cout << "Fire:" << to_string(input.GetMousePosition()) << '\n';
        //检查鼠标位置增量
        if (input.GetKey(KeyCode::LeftShift))
        {
            std::cout << "LeftShift:" << to_string(input.GetMouseMoveDelta()) << '\n';
            windowPosition += int2(input.GetMouseMoveDelta().x, input.GetMouseMoveDelta().y);
            glfwSetWindowPos(Window::GetGlfwWindow(), windowPosition.x, windowPosition.y);
        }
        //检查输入区域功能
        if (input.GetMouseButtonDown(MouseButton::Right))
            input.SetFocusArea({Window::GetMousePosition(), float2(std::numeric_limits<float>::max())});
        //检查光标隐藏
        if (input.GetMouseButtonDown(MouseButton::Right))
        {
            Cursor::SetLockState(true);
            Cursor::SetVisible(false);
        }
        else if (input.GetMouseButtonUp(MouseButton::Right))
        {
            Cursor::SetLockState(false);
            Cursor::SetVisible(true);
        }
        //检查窗口大小修改
        if (input.GetKeyDown(KeyCode::Minus))
        {
            resolution /= 2;
            Window::SetResolution(static_cast<int2>(resolution));
        }
        else if (input.GetKeyDown(KeyCode::Equals))
        {
            resolution *= 2;
            Window::SetResolution(static_cast<int2>(resolution));
        }
        //检查全屏功能
        if (input.GetKeyDown(KeyCode::F11))
            Window::SetFullScreen(!Window::GetFullScreen());
    }
};
Gleam_MakeSystem(InputTest)
Gleam_AddRuntimeSystems(InputTest)

Gleam_Main