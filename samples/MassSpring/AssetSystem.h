#pragma once
#include "GameUISystem.h"
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/System.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"

Gleam_MakeArchetypeChild(CameraArchetype2, Gleam::CameraArchetype, Gleam::ScreenToClip, Gleam::ScreenToWorld)

class AssetSystem : public Gleam::System
{
public:
    Gleam::Entity GetCameraEntity() const;

private:
    friend class LogicSystem;

    Gleam::Entity cameraEntity = Gleam::Entity::Null;
    Gleam::Entity centerParticle = Gleam::Entity::Null;
    std::vector<Gleam::Entity> colliders;

    void Start() override;
};
Gleam_MakeGlobalSystem(AssetSystem)