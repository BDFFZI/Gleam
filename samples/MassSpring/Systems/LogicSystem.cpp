#include "LogicSystem.h"

#include "RenderingSystem.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"
#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightWindow/Runtime/Time.h"


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

    if (Input::GetKey(KeyCode::Space))
        Time::SetTimeScale(1);
    else
        Time::SetTimeScale(0);

    if (Input::GetMouseButtonDown(MouseButton::Left))
    {
        float2 mousePositionWS = Rendering::ScreenToWorldPoint(Input::GetMousePosition());
        View<Point>::Each([mousePositionWS](const Entity entity, const Point& point)
        {
            if (distance(point.position, mousePositionWS) < 1)
                captivePoint = entity;
        });
    }
}
void Light::LogicSystem::FixedUpdate()
{
    if (captivePoint != Entity::Null)
        World::GetComponent<MassPointPhysics>(captivePoint).force = 0;
}
