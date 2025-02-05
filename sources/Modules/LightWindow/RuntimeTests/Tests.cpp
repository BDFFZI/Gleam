#include <iostream>
#include <gtest/gtest.h>

#include "LightEngine/Runtime/Engine.h"
#include "LightMath/Runtime/LinearAlgebra/VectorMath.h"
#include "LightWindow/Runtime/Cursor.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"

using namespace Light;

SystemEvent systemEvent = {"WindowTest", PostUpdateSystem};
int2 windowPosition;
float2 resolution;

Light_MakeInitEvent()
{
    systemEvent.onStart = [&]
    {
        windowPosition = Window->GetWindowPosition();
        resolution = static_cast<float2>(Window->GetResolution());
    };
    systemEvent.onUpdate = [&]
    {
        //检查WASD输入
        float2 moveInput = 0;
        if (Input->GetKey(KeyCode::W))
            moveInput.y = 1;
        else if (Input->GetKey(KeyCode::S))
            moveInput.y = -1;
        if (Input->GetKey(KeyCode::A))
            moveInput.x = -1;
        else if (Input->GetKey(KeyCode::D))
            moveInput.x = 1;
        if (any(moveInput))
            std::cout << "Move:" << to_string(moveInput) << '\n';
        //检查鼠标左键和鼠标位置输入
        if (Input->GetMouseButton(MouseButton::Left))
            std::cout << "Fire:" << to_string(Input->GetMousePosition()) << '\n';
        //检查鼠标位置增量
        if (Input->GetKey(KeyCode::LeftShift))
        {
            std::cout << "LeftShift:" << to_string(Input->GetMouseMoveDelta()) << '\n';
            windowPosition += int2(Input->GetMouseMoveDelta().x, Input->GetMouseMoveDelta().y);
            glfwSetWindowPos(Window->GetGlfwWindow(), windowPosition.x, windowPosition.y);
        }
        //检查输入区域功能
        if (Input->GetMouseButtonDown(MouseButton::Right))
            Input->SetFocusArea({Window->GetMousePosition(), float2(std::numeric_limits<float>::max())});
        //检查光标隐藏
        if (Input->GetMouseButtonDown(MouseButton::Right))
        {
            Cursor->SetLockState(true);
            Cursor->SetVisible(false);
        }
        else if (Input->GetMouseButtonUp(MouseButton::Right))
        {
            Cursor->SetLockState(false);
            Cursor->SetVisible(true);
        }
        //检查窗口大小修改
        if (Input->GetKeyDown(KeyCode::Minus))
        {
            resolution /= 2;
            Window->SetResolution(static_cast<int2>(resolution));
        }
        else if (Input->GetKeyDown(KeyCode::Equals))
        {
            resolution *= 2;
            Window->SetResolution(static_cast<int2>(resolution));
        }
        //检查全屏功能
        if (Input->GetKeyDown(KeyCode::F11))
            Window->SetFullScreen(!Window->GetFullScreen());
    };

    World::AddSystem(&systemEvent);
}