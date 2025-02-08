#include "LogicSystem.h"

#include "Archetype.h"
#include "LightWindow/Runtime/System/InputSystem.h"
#include "LightWindow/Runtime/Window.h"
#include "LightECS/Runtime/View.h"
#include "LightEngine/Editor/System/InspectorWindow.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightWindow/Runtime/Time.h"

#include "Public/Component.h"
#include "Rendering/RenderingSystem.h"

using namespace Light;

void LogicSystem::OnMovePoint()
{
    if (InputSystem->GetMouseButtonDown(MouseButton::Left))
    {
        fixedPoint = coveringPoint;
        InspectorWindow->target = fixedPoint;
    }

    if (InputSystem->GetMouseButtonUp(MouseButton::Left))
        fixedPoint = Entity::Null;

    if (fixedPoint != Entity::Null)
        World::GetComponent<Point>(fixedPoint).position = mousePositionWS;
}
void LogicSystem::OnCreatePoint() const
{
    if (InputSystem->GetMouseButtonDown(MouseButton::Left))
    {
        const Entity entity = World::AddEntity(MassPointArchetype);
        World::SetComponents(entity, Point{mousePositionWS});
        InspectorWindow->target = entity;
    }
}

std::vector<Entity> lines;
void LogicSystem::OnDeletePoint()
{
    if (InputSystem->GetMouseButtonDown(MouseButton::Left) && coveringPoint != Entity::Null)
    {
        lines.clear();
        View<SpringPhysics>::Each([this](const Entity entity, SpringPhysics& springPhysics)
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
        if (InputSystem->GetMouseButtonDown(MouseButton::Left))
        {
            if (coveringPoint != Entity::Null)
            {
                springPointA = coveringPoint;
                tempLine = World::AddEntity(LineArchetype);
            }
        }
    }
    else
    {
        if (InputSystem->GetMouseButtonDown(MouseButton::Left))
        {
            if (coveringPoint != Entity::Null && coveringPoint != springPointA)
            {
                Point pointA = World::GetComponent<Point>(springPointA);
                Point pointB = World::GetComponent<Point>(coveringPoint);
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(
                    entity, SpringPhysics{
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
        World::SetComponents(tempLine, Segment{pointA.position, mousePositionWS});
    }
}

void LogicSystem::Update()
{
    //根据当前数值状态运行游戏逻辑
    if (InputSystem->GetKeyDown(KeyCode::Space))
        simulating = true;
    if (InputSystem->GetKeyUp(KeyCode::Space))
        simulating = false;
    //获取鼠标位置
    mousePositionWS = RenderingSystem.ScreenToWorldPoint(InputSystem->GetMousePosition());
    //获取当前鼠标覆盖的顶点
    coveringPoint = Entity::Null;
    View<Point>::Each([this](const Entity entity, const Point& point)
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

    Time->SetTimeScale(simulating ? 1 : 0);
}