#include "LogicSystem.h"

#include <imgui.h>

#include "PhysicsSystem.h"
#include "RenderingSystem.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"
#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"
#include "../Editor/InspectorWindow.h"
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

    mousePositionWS = Rendering::ScreenToWorldPoint(Input::GetMousePosition());

    if (Input::GetMouseButtonDown(MouseButton::Left))
    {
        View<Point>::Each([](const Entity entity, const Point& point)
        {
            if (distance(point.position, mousePositionWS) < 1)
            {
                captivePoint = entity;
                InspectorWindow::target = entity;
            }
        });
    }
    if (Input::GetMouseButtonUp(MouseButton::Left))
        captivePoint = Entity::Null;

    if (captivePoint != Entity::Null)
        World::GetComponent<Point>(captivePoint).position = mousePositionWS;

    ImGui::Text(std::format("MousePositionWS:{}", to_string(mousePositionWS)).c_str());
    int entity = static_cast<int>(captivePoint);
    ImGui::Text(std::format("CaptivePoint:{}", entity).c_str());
}
void Light::LogicSystem::FixedUpdate()
{
    if (captivePoint != Entity::Null)
    {
        Point* point;
        MassPointPhysics* massPointPhysics;
        World::GetComponents(captivePoint, &point, &massPointPhysics);
        massPointPhysics->force = 0;
        massPointPhysics->velocity = 0;
    }
}
