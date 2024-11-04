#include <iostream>

#include "LightMath/Runtime/VectorMath.hpp"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Time.h"
#include "LightWindow/Runtime/Window.h"

using namespace Light;

//TODO：发现问题：长按检测有延迟，鼠标不支持长按
void main()
{
    Window::Initialize();

    static float2 moveInput;
    static bool fireInput;

    static InputHandler inputEvent;
    inputEvent.name = "Test";
    inputEvent.event = []
    {
        if (Input::GetKey(KeyCode::W))
            moveInput.y = 1;
        else if (Input::GetKey(KeyCode::S))
            moveInput.y = -1;
        else
            moveInput.y = 0;

        if (Input::GetKey(KeyCode::A))
            moveInput.x = -1;
        else if (Input::GetKey(KeyCode::D))
            moveInput.x = 1;
        else
            moveInput.x = 0;

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
    Window::SetWindowUpdateEvent([]
    {
        if (fireInput)
        {
            std::cout
                << Time::GetTime() << "\t"
                << to_string(moveInput)
                << '\n';
        }
    });

    Window::Start();
}
