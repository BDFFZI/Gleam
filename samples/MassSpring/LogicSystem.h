#pragma once
#include <iostream>

#include "LightECS/Runtime/Entity.h"
#include "LightECS/Runtime/System.h"
#include "LightMassSpring/Runtime/System/ForceSystem.h"
#include "LightMassSpring/Runtime/System/PhysicsSystem.h"
#include "LightMassSpring/Runtime/System/PositionSystem.h"
#include "LightWindow/Runtime/System/InputSystem.h"

enum class EditMode
{
    MovePoint,
    CreatePoint,
    DeletePoint,
    CreateSpring,
};

class LogicSystem : public Light::System
{
public:
    EditMode& GetEditMode() { return editMode; }
    bool& GetSimulating() { return simulating; }
    Light::Entity GetFixedPoint() const { return fixedPoint; }
    Light::Entity GetCoveringPoint() const { return coveringPoint; }
    Light::float3 GetMousePositionWS() const { return mousePositionWS; }

private:
    EditMode editMode = EditMode::MovePoint;
    bool simulating = false;
    Light::float3 mousePositionWS = 0;
    Light::Entity coveringPoint = Light::Entity::Null; //鼠标当前位置所在的点
    Light::Entity fixedPoint = Light::Entity::Null; //移动点模式下的移动点
    Light::Entity springPointA = Light::Entity::Null; //创建弹簧时的弹簧A点
    Light::Entity tempLine = Light::Entity::Null; //创建弹簧时临时的可视化线
    Light::SystemEvent fixedPointSystem = {"FixedPointSystem",
        Light::PhysicsSystem, Light::ForceSystem->GetOrder(), Light::PositionSystem->GetOrder(),};

    void OnMovePoint();
    void OnCreatePoint() const;
    void OnDeletePoint();
    void OnCreateSpring();

    void Start() override;
    void Stop() override;
    void Update() override;
};
Light_MakeSystemInstance(LogicSystem)