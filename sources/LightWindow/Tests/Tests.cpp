#include <iostream>

#include "LightMath/Runtime/VectorMath.hpp"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Time.h"
#include "LightWindow/Runtime/Window.h"

using namespace Light;

void main()
{
    int width = 1920 / 2;
    int height = 1080 / 2;
    Window::Initialize("Window", width, height, false);

    int2 position;
    glfwGetWindowPos(Window::GetGlfwWindow(), &position.x, &position.y);

    static float2 moveInput;
    static bool fireInput;

    static InputHandler inputEvent;
    inputEvent.name = "Test";
    inputEvent.inputCallback = []
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
    Window::SetWindowUpdateEvent([&width,&height,&position]
    {
        if (any(moveInput))
            std::cout << "Move:" << to_string(moveInput) << '\n';
        if (fireInput)
            std::cout << "Fire:" << to_string(Input::GetMousePosition()) << '\n';
        if (Input::GetKey(KeyCode::T))
            std::cout << std::format("Time:{:f}\tDeltaTime:{:f}\n", Time::GetTime(), Time::GetDeltaTime());
        if (Input::GetKey(KeyCode::LeftShift))
        {
            position += static_cast<int2>(float2(Input::GetMouseMoveDelta().x, -Input::GetMouseMoveDelta().y));
            glfwSetWindowPos(Window::GetGlfwWindow(), position.x, position.y);
        }


        if (Input::GetKeyDown(KeyCode::Minus))
        {
            width /= 2;
            height /= 2;
            Window::SetResolution(width, height);
        }
        else if (Input::GetKeyDown(KeyCode::Equals))
        {
            width *= 2;
            height *= 2;
            Window::SetResolution(width, height);
        }
        if (Input::GetKeyDown(KeyCode::F11))
            Window::SetFullScreen(!Window::GetFullScreen());
    });

    Window::Start();
}
