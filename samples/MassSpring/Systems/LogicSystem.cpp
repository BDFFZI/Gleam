#include "LogicSystem.h"

#include <imgui.h>
#include <magic_enum.hpp>

#include "PhysicsSystem.h"
#include "RenderingSystem.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"
#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"
#include "../Editor/EditorUIUtility.h"
#include "../Editor/InspectorWindow.h"
#include "LightWindow/Runtime/Time.h"

using namespace Light;

void LogicSystem::Start()
{
    Physics::AddFixedUpdate(FixedUpdate);
}
void LogicSystem::Stop()
{
    Physics::RemoveFixedUpdate(FixedUpdate);
}


void LogicSystem::UpdateUI()
{
    const char* editModeOptions[] = {
        magic_enum::enum_name(static_cast<EditMode>(0)).data(),
        magic_enum::enum_name(static_cast<EditMode>(1)).data(),
        magic_enum::enum_name(static_cast<EditMode>(2)).data(),
        magic_enum::enum_name(static_cast<EditMode>(3)).data(),
    };
    ImGui::Combo("EditMode", reinterpret_cast<int*>(&editMode), editModeOptions, std::size(editModeOptions));

    ImGui::Checkbox("Simulating", &simulating);
    if (Input::GetKeyDown(KeyCode::Space))
        simulating = true;
    if (Input::GetKeyUp(KeyCode::Space))
        simulating = false;

    ImGui::Text("SelectingPoint");
    ImGui::SameLine();
    EditorUIUtility::DrawEntityButton(selectingPoint);

    if (ImGui::Button("ExitGame"))
        Window::Stop();
}
void LogicSystem::OnMovePoint()
{
    if (Input::GetMouseButtonDown(MouseButton::Left))
    {
        View<Point>::Each([](const Entity entity, const Point& point)
        {
            if (distance(point.position, mousePositionWS) < 1)
            {
                selectingPoint = entity;
                InspectorWindow::target = entity;
            }
        });
    }

    if (Input::GetMouseButtonUp(MouseButton::Left))
        selectingPoint = Entity::Null;

    if (selectingPoint != Entity::Null)
        World::GetComponent<Point>(selectingPoint).position = mousePositionWS;
}
void LogicSystem::OnCreatePoint()
{
    // World::AddEntity(massPointArchetype,)
}
void LogicSystem::OnDeletePoint()
{
}
void LogicSystem::OnCreateSpring()
{
}

void LogicSystem::Update()
{
    UpdateUI();

    Time::SetTimeScale(simulating ? 1 : 0);

    mousePositionWS = Rendering::ScreenToWorldPoint(Input::GetMousePosition());
    switch (editMode)
    {
    case EditMode::MovePoint:
        OnMovePoint();
    case EditMode::CreatePoint:
        OnCreatePoint();
        break;
    case EditMode::DeletePoint:
        OnDeletePoint();
        break;
    case EditMode::CreateSpring:
        OnCreateSpring();
        break;
    }
}
void LogicSystem::FixedUpdate()
{
    if (editMode == EditMode::MovePoint)
    {
        if (selectingPoint != Entity::Null)
        {
            Point* point;
            MassPointPhysics* massPointPhysics;
            World::GetComponents(selectingPoint, &point, &massPointPhysics);
            massPointPhysics->force = 0;
            massPointPhysics->velocity = 0;
        }
    }
}
