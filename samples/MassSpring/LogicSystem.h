#pragma once
#include <iostream>

#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/System.h"
#include "GleamMassSpring/Runtime/System/ForceSystem.h"
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"
#include "GleamMassSpring/Runtime/System/PositionSystem.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

enum class EditMode
{
    MoveParticle,
    CreateParticle,
    DeleteParticle,
    CreateSpring,
};

class LogicSystem : public Gleam::System
{
    friend class GameUISystem;

    EditMode editMode = EditMode::MoveParticle;
    bool simulating = true;
    Gleam::float3 mousePositionWS = 0;
    Gleam::Entity coveringParticle = Gleam::Entity::Null; //鼠标当前位置所在的点
    Gleam::Entity fixedParticle = Gleam::Entity::Null; //移动点模式下的移动点
    Gleam::Entity springParticleA = Gleam::Entity::Null; //创建弹簧时的弹簧A点
    Gleam::Entity tempLine = Gleam::Entity::Null; //创建弹簧时临时的可视化线
    Gleam::SystemEvent physicsSystemEvent = {"PhysicsSystemEvent", Gleam::PhysicsSystem, MinOrder, MinOrder,};

    float drag = 0.01f;
    float mass = 1;
    float elasticity = 1000;
    float colliderFriction = 0.5f;
    float colliderElasticity = 0.5f;
    bool test;

    void OnMoveParticle();
    void OnCreateParticle() const;
    void OnDeleteParticle();
    void OnCreateSpring();

    void Start() override;
    void Stop() override;
    void Update() override;
};
Gleam_MakeGlobalSystem(LogicSystem)