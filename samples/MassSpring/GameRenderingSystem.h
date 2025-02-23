#pragma once
#include "LogicSystem.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMassSpring/Runtime/Component/Spring.h"
#include "GleamRendering/Runtime/Component/LinesMesh.h"
#include "GleamRendering/Runtime/Component/PointsMesh.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

class GameRenderingSystem : public Gleam::System
{
public:
    GameRenderingSystem(): System(Gleam::RenderingSystem, Gleam::OrderRelation::Before)
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