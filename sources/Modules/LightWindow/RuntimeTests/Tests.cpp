#include <iostream>
#include <gtest/gtest.h>

#include "LightEngine/Runtime/Engine.h"
#include "LightMath/Runtime/VectorMath.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Time.h"
#include "LightWindow/Runtime/Window.h"

using namespace Light;

int main()
{
    int2 mousePosition;
    int2 resolution;

    SystemEvent systemEvent = {"WindowTest", SimulationSystem};
    systemEvent.onStart = [&]
    {
        glfwGetWindowPos(Window->GetGlfwWindow(), &mousePosition.x, &mousePosition.y);
        resolution = Window->GetResolution();
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
        //检查时间
        if (Input->GetKey(KeyCode::T))
            std::cout << std::format("Time:{:f}\tDeltaTime:{:f}\n", Time->GetTime(), Time->GetDeltaTime());
        //检查鼠标位置增量
        if (Input->GetKey(KeyCode::LeftShift))
        {
            mousePosition += static_cast<int2>(float2(Input->GetMouseMoveDelta().x, Input->GetMouseMoveDelta().y));
            glfwSetWindowPos(Window->GetGlfwWindow(), mousePosition.x, mousePosition.y);
        }
        //检查输入区域功能
        if (Input->GetMouseButtonDown(MouseButton::Right))
            Input->SetFocusArea({Window->GetMousePosition(), float2(std::numeric_limits<float>::max())});
        //检查窗口大小修改
        if (Input->GetKeyDown(KeyCode::Minus))
        {
            resolution /= 2;
            Window->SetResolution(resolution);
        }
        else if (Input->GetKeyDown(KeyCode::Equals))
        {
            resolution *= 2;
            Window->SetResolution(resolution);
        }
        //检查全屏功能
        if (Input->GetKeyDown(KeyCode::F11))
            Window->SetFullScreen(!Window->GetFullScreen());
    };

    World::AddSystem(&systemEvent);
    Engine::Start();
    return 0;
}