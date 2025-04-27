#pragma once
#include "GameUISystem.h"
#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

Gleam_MakeArchetypeChild(CameraArchetype2, Gleam::CameraArchetype, Gleam::ScreenToClip, Gleam::ScreenToWorld)

class AssetSystem : public Gleam::System<>
{
public:
    Gleam::Entity GetCameraEntity() const;

private:
    friend class LogicSystem;
    friend class PhysicsLogicSystem;

    Gleam::Entity cameraEntity = Gleam::Entity::Null;
    Gleam::Entity centerParticle = Gleam::Entity::Null;
    std::vector<Gleam::Entity> colliders;

    void Start() override;
};

#ifdef GleamEngineEditor
Gleam_MakeEditorSystem(AssetSystem)
#else
Gleam_MakeRuntimeSystem(AssetSystem)
#endif