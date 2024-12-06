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
    Input::PushInputHandler(inputHandler);
}
void LogicSystem::Stop()
{
    Physics::RemoveFixedUpdate(FixedUpdate);
    Input::PopInputHandler(inputHandler);
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
    EditorUIUtility::DrawEntityButton(coveringPoint);

    if (ImGui::Button("ExitGame"))
        Window::Stop();
}
void LogicSystem::OnMovePoint()
{
    if (Input::GetMouseButtonDown(MouseButton::Left))
    {
        movingPoint = coveringPoint;
        InspectorWindow::target = movingPoint;
    }

    if (Input::GetMouseButtonUp(MouseButton::Left))
        movingPoint = Entity::Null;

    if (movingPoint != Entity::Null)
        World::GetComponent<Point>(movingPoint).position = mousePositionWS;
}
void LogicSystem::OnCreatePoint()
{
    if (Input::GetMouseButtonDown(MouseButton::Left))
    {
        Entity entity = World::AddEntity(massPointArchetype, Point{mousePositionWS});
        InspectorWindow::target = entity;
    }
}
void LogicSystem::OnDeletePoint()
{
    if (Input::GetMouseButtonDown(MouseButton::Left))
    {
        World::RemoveEntity(coveringPoint);
    }
}
void LogicSystem::OnCreateSpring()
{
    // if(springPoints[0] == Entity::Null)
    // {
    //     if (Input::GetMouseButtonDown(MouseButton::Left))
    //     {
    //         
    //     }
    // }
}

void LogicSystem::Update()
{
    //将输入回调处理权释放给UI
    if (Input::GetKeyDown(KeyCode::LeftAlt))
        Input::PopInputHandler(inputHandler);
    if (Input::GetKeyUp(KeyCode::LeftAlt))
        Input::PushInputHandler(inputHandler);
    //处理UI输入
    if (Input::GetKey(KeyCode::LeftAlt))
        UpdateUI();

    //根据当前数值状态运行游戏逻辑
    Time::SetTimeScale(simulating ? 1 : 0);
    if (Input::TopInputHandler() == inputHandler)
    {
        //获取鼠标位置
        mousePositionWS = Rendering::ScreenToWorldPoint(Input::GetMousePosition());
        //获取当前鼠标覆盖的顶点
        coveringPoint = Entity::Null;
        View<Point>::Each([](const Entity entity, const Point& point)
        {
            if (distance(point.position, mousePositionWS) < 1)
                coveringPoint = entity;
        });

        switch (editMode)
        {
        case EditMode::MovePoint:
            OnMovePoint();
            break;
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
}
void LogicSystem::FixedUpdate()
{
    if (editMode == EditMode::MovePoint)
    {
        if (movingPoint != Entity::Null)
        {
            Point* point;
            MassPointPhysics* massPointPhysics;
            World::GetComponents(coveringPoint, &point, &massPointPhysics);
            massPointPhysics->force = 0;
            massPointPhysics->velocity = 0;
        }
    }
}
