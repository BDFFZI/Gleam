#pragma once
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/LinesMesh.h"
#include "LightRendering/Runtime/Component/PointsMesh.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightECS/Runtime/Archetype.h"

namespace Light
{
    Light_MakeArchetype(CameraArchetype, LocalTransform, LocalToWorld, Camera, WorldToClip)
    Light_MakeArchetype(PointRendererArchetype, LocalTransform, LocalToWorld, PointsMesh, Renderer)
    Light_MakeArchetype(LineRendererArchetype, LocalTransform, LocalToWorld, LinesMesh, Renderer)
}