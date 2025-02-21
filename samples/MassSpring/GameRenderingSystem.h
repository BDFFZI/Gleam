#pragma once
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMassSpring/Runtime/Component/Spring.h"
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"
#include "GleamRendering/Runtime/Component/LinesMesh.h"
#include "GleamRendering/Runtime/Component/PointsMesh.h"

class GameRenderingSystem : public Gleam::System
{
public:
    GameRenderingSystem(): System(Gleam::PhysicsSystem, Gleam::OrderRelation::After)
    {
    }

private:
    std::vector<Gleam::Point> particles;
    std::vector<Gleam::Segment> springs;

    Gleam::Entity pointsRenderer = Gleam::Entity::Null;
    Gleam::Entity linesRenderer = Gleam::Entity::Null;

    void Start() override;
    void Update() override;
};
Gleam_MakeGlobalSystem(GameRenderingSystem)