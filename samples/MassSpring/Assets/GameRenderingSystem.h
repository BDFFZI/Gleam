#pragma once
#include "LogicSystem.h"
#include "GleamRendering/Runtime/Entity/LinesMesh.h"
#include "GleamRendering/Runtime/Entity/PointsMesh.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

class GameRenderingSystem : public Gleam::RelativeSystem<Gleam::RenderingSystem, Gleam::SystemRelation::Before>
{
    std::vector<Gleam::Point> particles;
    std::vector<Gleam::Segment> springs;

    Gleam::Entity pointsRenderer = Gleam::Entity::Null;
    Gleam::Entity linesRenderer = Gleam::Entity::Null;

    void Start() override;
    void Update() override;
};

#ifdef GleamEngineEditor
Gleam_MakeEditorSystem(GameRenderingSystem)
#else
Gleam_MakeRuntimeSystem(GameRenderingSystem)
#endif