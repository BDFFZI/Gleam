#pragma once
#include "GameUISystem.h"
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/System.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"

Gleam_MakeArchetypeChild(CameraArchetype2, Gleam::CameraArchetype, Gleam::ScreenToWorld)

class AssetSystem : public Gleam::System
{
public:
    Gleam::Entity GetCameraEntity() const;

private:
    Gleam::Entity cameraEntity = Gleam::Entity::Null;

    void Start() override;
};
Gleam_MakeGlobalSystem(AssetSystem)

