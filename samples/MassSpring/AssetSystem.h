#pragma once
#include "GameUISystem.h"
#include "LightECS/Runtime/Archetype.h"
#include "LightECS/Runtime/Entity.h"
#include "LightECS/Runtime/System.h"
#include "LightRendering/Runtime/Entity/Archetype.h"

Light_MakeArchetypeChild(CameraArchetype2, Light::CameraArchetype, Light::ScreenToWorld)

class AssetSystem : public Light::System
{
public:
    Light::Entity GetCameraEntity() const;

private:
    Light::Entity cameraEntity = Light::Entity::Null;

    void Start() override;
};
Light_MakeSystemInstance(AssetSystem)

