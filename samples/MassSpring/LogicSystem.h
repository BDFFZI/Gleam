#pragma once
#include "LightEngine/Runtime/ECS/System.h"
#include "LightEngine/Runtime/ECS/_Concept.hpp"
#include "LightWindow/Runtime/Input.h"
#include "Physics/PhysicsSystem.h"
#include "Public/SimulationSystem.h"


enum class EditMode:uint8_t
{
    MovePoint,
    CreatePoint,
    DeletePoint,
    CreateSpring,
};

class LogicSystem : public Light::System
{
public:
    LogicSystem(): System(&Light::SimulationSystem, Light::PhysicsSystem.order, RightOrder)
    {
    }

    EditMode& GetEditMode() { return editMode; }
    bool& GetSimulating() { return simulating; }
    Light::InputHandler& GetInputHandler() { return inputHandler; }
    Light::Entity GetFixedPoint() const { return fixedPoint; }
    Light::Entity GetCoveringPoint() const { return coveringPoint; }

private:
    EditMode editMode = EditMode::MovePoint;
    bool simulating = false;
    Light::float2 mousePositionWS = 0;
    Light::Entity coveringPoint = Light::Entity::Null; //鼠标当前位置所在的点
    Light::Entity fixedPoint = Light::Entity::Null; //移动点模式下的移动点
    Light::Entity springPointA = Light::Entity::Null; //创建弹簧时的弹簧A点
    Light::Entity tempLine = Light::Entity::Null; //创建弹簧时临时的可视化线
    Light::InputHandler inputHandler = {"LogicSystemInputHandler"};

    void OnMovePoint();
    void OnCreatePoint() const;
    void OnDeletePoint();
    void OnCreateSpring();

    void Start() override;
    void Stop() override;
    void Update() override;
};
inline LogicSystem LogicSystem = {};
