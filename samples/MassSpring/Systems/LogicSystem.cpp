#include "LogicSystem.h"
#include "PhysicsSystem.h"
#include "RenderingSystem.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"
#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"
#include "../Editor/EditorUIUtility.h"
#include "../Editor/InspectorWindow.h"
#include "LightWindow/Runtime/Time.h"
#include "LightUI/Runtime/UI.h"
#include <magic_enum.hpp>

using namespace Light;

void LogicSystem::Start()
{
    Physics::AddFixedUpdate(FixedUpdate);
    Input::PushInputHandler(inputHandler);
}
void LogicSystem::Stop()
{
    Physics::RemoveFixedUpdate(FixedUpdate);
    if (Input::TopInputHandler() == inputHandler)
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
        fixedPoint = coveringPoint;
        InspectorWindow::target = fixedPoint;
    }

    if (Input::GetMouseButtonUp(MouseButton::Left))
        fixedPoint = Entity::Null;

    if (fixedPoint != Entity::Null)
        World::GetComponent<Point>(fixedPoint).position = mousePositionWS;
}
void LogicSystem::OnCreatePoint()
{
    if (Input::GetMouseButtonDown(MouseButton::Left))
    {
        Entity entity = World::AddEntity(massPointArchetype, Point{mousePositionWS});
        InspectorWindow::target = entity;
    }
}

std::vector<Entity> lines;
void LogicSystem::OnDeletePoint()
{
    if (Input::GetMouseButtonDown(MouseButton::Left) && coveringPoint != Entity::Null)
    {
        lines.clear();
        View<SpringPhysics>::Each([](const Entity entity, SpringPhysics& springPhysics)
        {
            if (springPhysics.pointA == coveringPoint || springPhysics.pointB == coveringPoint)
                lines.push_back(entity);
        });
        for (Entity entity : lines)
            World::RemoveEntity(entity);
        World::RemoveEntity(coveringPoint);
    }
}
void LogicSystem::OnCreateSpring()
{
    if (springPointA == Entity::Null)
    {
        if (Input::GetMouseButtonDown(MouseButton::Left))
        {
            if (coveringPoint != Entity::Null)
            {
                springPointA = coveringPoint;
                tempLine = World::AddEntity(lineArchetype);
            }
        }
    }
    else
    {
        if (Input::GetMouseButtonDown(MouseButton::Left))
        {
            if (coveringPoint != Entity::Null && coveringPoint != springPointA)
            {
                Point pointA = World::GetComponent<Point>(springPointA);
                Point pointB = World::GetComponent<Point>(coveringPoint);
                World::AddEntity(
                    springArchetype,
                    SpringPhysics{
                        springPointA,
                        coveringPoint,
                        distance(pointA.position, pointB.position),
                    });
            }

            springPointA = Entity::Null;
            World::RemoveEntity(tempLine);
        }
    }

    if (tempLine != Entity::Null)
    {
        Point pointA = World::GetComponent<Point>(springPointA);
        World::SetComponents(tempLine, Line{pointA.position, mousePositionWS});
    }
}

void LogicSystem::Update()
{
    //将输入回调处理权释放给UI
    if (Input::GetKeyDown(KeyCode::LeftAlt))
    {
        if (Input::TopInputHandler() == inputHandler)
            Input::PopInputHandler(inputHandler);
        else
            Input::PushInputHandler(inputHandler);
    }
    //处理UI输入
    if (Input::TopInputHandler() != inputHandler)
        UpdateUI();

    //根据当前数值状态运行游戏逻辑
    if (Input::TopInputHandler() == inputHandler)
    {
        if (Input::GetKeyDown(KeyCode::Space))
            simulating = true;
        if (Input::GetKeyUp(KeyCode::Space))
            simulating = false;
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
        case EditMode::MovePoint: OnMovePoint();
            break;
        case EditMode::CreatePoint: OnCreatePoint();
            break;
        case EditMode::DeletePoint: OnDeletePoint();
            break;
        case EditMode::CreateSpring: OnCreateSpring();
            break;
        }
    }

    Time::SetTimeScale(simulating ? 1 : 0);
}
void LogicSystem::FixedUpdate()
{
    if (fixedPoint != Entity::Null)
    {
        Point* point;
        MassPointPhysics* massPointPhysics;
        World::GetComponents(fixedPoint, &point, &massPointPhysics);
        massPointPhysics->force = 0;
        massPointPhysics->velocity = 0;
    }
}
