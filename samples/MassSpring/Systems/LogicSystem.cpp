#include "LogicSystem.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"
#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"


void Light::LogicSystem::Start()
{
    Physics::AddFixedUpdate(FixedUpdate);
}
void Light::LogicSystem::Stop()
{
    Physics::RemoveFixedUpdate(FixedUpdate);
}
void Light::LogicSystem::Update()
{
    if (Input::GetKeyDown(KeyCode::Esc))
        Window::Stop();
    if (Input::GetMouseButtonDown(MouseButton::Left))
    {
        View<Point>::Each([ mousePosition =Input::GetMousePosition()](Entity entity, Point& p)
        {
            if (all(p.position == mousePosition))
                captivePoint = entity;
        });
    }

    // if (Input::GetKey(KeyCode::Space))
    //     Time::SetTimeScale(1);
    // else
    //     Time::SetTimeScale(0);
}
void Light::LogicSystem::FixedUpdate()
{
    View<Point, MassPointPhysics>::Each([](Point& point, MassPointPhysics& massPointPhysics)
    {
        if (Input::GetMouseButton(MouseButton::Left))
            std::cout << to_string(Input::GetMousePosition()) << std::endl;
    });
}
