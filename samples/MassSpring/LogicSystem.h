#pragma once
#include "LightECS/Runtime/Entity.h"
#include "LightECS/Runtime/System.h"
#include "LightWindow/Runtime/Input.h"
#include "Physics/PhysicsSystem.h"

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
    EditMode& GetEditMode() { return editMode; }
    bool& GetSimulating() { return simulating; }
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

    void OnMovePoint();
    void OnCreatePoint() const;
    void OnDeletePoint();
    void OnCreateSpring();

    void Update() override;
};
inline LogicSystem LogicSystem = {};
