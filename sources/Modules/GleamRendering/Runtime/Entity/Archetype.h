#pragma once
#include "GleamRendering/Runtime/Component/Camera.h"
#include "GleamRendering/Runtime/Component/LinesMesh.h"
#include "GleamRendering/Runtime/Component/PointsMesh.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamECS/Runtime/Archetype.h"

namespace Gleam
{
    Gleam_MakeArchetype(CameraArchetype, LocalTransform, LocalToWorld, Camera, WorldToClip)
    Gleam_MakeArchetype(PointRendererArchetype, LocalTransform, LocalToWorld, PointsMesh, Renderer)
    Gleam_MakeArchetype(LineRendererArchetype, LocalTransform, LocalToWorld, LinesMesh, Renderer)
}