#include <iostream>

#include "LightMath/Runtime/VectorMath.hpp"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Time.h"
#include "LightWindow/Runtime/Window.h"

using namespace Light;

void main()
{
    int width = 800;
    int height = 600;
    Window::Initialize("Window", width, height);

    static float2 moveInput;
    static bool fireInput;

    static InputHandler inputEvent;
    inputEvent.name = "Test";
    inputEvent.event = []
    {
        moveInput = 0;
        if (Input::GetKey(KeyCode::W))
            moveInput.y = 1;
        else if (Input::GetKey(KeyCode::S))
            moveInput.y = -1;
        if (Input::GetKey(KeyCode::A))
            moveInput.x = -1;
        else if (Input::GetKey(KeyCode::D))
            moveInput.x = 1;

        fireInput = Input::GetMouseButton(MouseButton::Left);
    };

    Window::SetWindowStartEvent([]()
    {
        Input::PushInputHandler(inputEvent);
    });
    Window::SetWindowStopEvent([]
    {
        Input::PopInputHandler(inputEvent);
    });
    Window::SetWindowUpdateEvent([&width,&height]
    {
        if (any(moveInput))
            std::cout << "Move:" << to_string(moveInput) << '\n';
        if (fireInput)
            std::cout << "Fire" << '\n';
        if (Input::GetKey(KeyCode::T))
            std::cout << std::format("Time:{:f}\tDeltaTime:{:f}\n", Time::GetTime(), Time::GetDeltaTime());
        if (Input::GetKeyDown(KeyCode::Minus))
        {
            width -= 50;
            height -= 50;
            Window::SetResolution(width, height);
        }
        else if (Input::GetKeyDown(KeyCode::Equals))
        {
            width += 50;
            height += 50;
            Window::SetResolution(width, height);
        }
    });

    Window::Start();
}
