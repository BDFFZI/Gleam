#include "LogicSystem.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"

void Light::LogicSystem::Update()
{
    if (Input::GetKeyDown(KeyCode::Esc))
    {
        Window::Stop();
    }
}
