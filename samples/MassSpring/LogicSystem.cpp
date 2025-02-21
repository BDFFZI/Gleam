#include "LogicSystem.h"

#include "AssetSystem.h"
#include "GleamWindow/Runtime/System/InputSystem.h"
#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/System/InspectorWindow.h"
#endif

using namespace Gleam;

void LogicSystem::OnMovePoint()
{
    if (InputSystem.GetMouseButtonDown(MouseButton::Left))
    {
        fixedPoint = coveringPoint;
#ifdef GleamEngineEditor
        InspectorWindow.SetTarget(fixedPoint);
#endif
    }
    else if (InputSystem.GetMouseButtonUp(MouseButton::Left))
        fixedPoint = Entity::Null;

    if (World::HasEntity(fixedPoint))
        World::GetComponent<Point>(fixedPoint).position = mousePositionWS;
}
void LogicSystem::OnCreatePoint() const
{
    if (InputSystem.GetMouseButtonDown(MouseButton::Left))
    {
        // ReSharper disable once CppDeclaratorNeverUsed
        Entity entity = PhysicsSystem::AddMassPoint(mousePositionWS, drag, mass);
#ifdef GleamEngineEditor
        InspectorWindow.SetTarget(entity);
#endif
    }
}

std::vector<Entity> lines;
void LogicSystem::OnDeletePoint()
{
    if (InputSystem.GetMouseButtonDown(MouseButton::Left) && coveringPoint != Entity::Null)
    {
        lines.clear();
        View<Spring>::Each([this](const Entity entity, Spring& springPhysics)
        {
            if (springPhysics.particleA == coveringPoint || springPhysics.particleB == coveringPoint)
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
        if (InputSystem.GetMouseButtonDown(MouseButton::Left))
        {
            if (coveringPoint != Entity::Null)
            {
                springPointA = coveringPoint;
                tempLine = World::AddEntity(LineRendererArchetype);
            }
        }
    }
    else
    {
        if (InputSystem.GetMouseButtonDown(MouseButton::Left))
        {
            if (coveringPoint != Entity::Null && coveringPoint != springPointA)
                PhysicsSystem::AddSpring(springPointA, coveringPoint, elasticity);

            springPointA = Entity::Null;
            World::RemoveEntity(tempLine);
        }
    }

    if (tempLine != Entity::Null)
    {
        Point pointA = World::GetComponent<Point>(springPointA);
        World::GetComponent<LinesMesh>(tempLine).lines = {
            Segment{pointA.position, mousePositionWS}
        };
    }
}
void LogicSystem::Start()
{
    fixedPointSystem.OnUpdate() = [this]
    {
        View<Point, Particle>::Each([this](const Entity entity, Point& point, Particle& particle)
        {
            if (entity == fixedPoint)
            {
                particle.force = 0;
            }
        });
    };
    World::AddSystem(fixedPointSystem);
}
void LogicSystem::Stop()
{
    World::RemoveSystem(fixedPointSystem);
}
void LogicSystem::Update()
{
    //根据当前数值状态运行游戏逻辑
    if (InputSystem.GetKeyDown(KeyCode::Space))
        simulating = true;
    if (InputSystem.GetKeyUp(KeyCode::Space))
        simulating = false;
    //获取鼠标位置
    ScreenToWorld screenToWorld = World::GetComponent<ScreenToWorld>(AssetSystem.GetCameraEntity());
    mousePositionWS = float3(mul(screenToWorld.value, float4(InputSystem.GetMousePosition(), 0, 1)).xy, 1);
    //获取当前鼠标覆盖的顶点
    coveringPoint = Entity::Null;
    View<Point>::Each([this](const Entity entity, const Point& point)
    {
        if (distance(point.position, mousePositionWS) < 2)
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

    TimeSystem.SetTimeScale(simulating ? 1 : 0);

    for (Entity collider : AssetSystem.colliders)
        World::SetComponents(collider, Collider{colliderFriction, colliderElasticity});
}