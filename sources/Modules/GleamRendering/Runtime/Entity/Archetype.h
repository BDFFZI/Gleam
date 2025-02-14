#pragma once
#include "GleamRendering/Runtime/Component/Camera.h"
#include "GleamRendering/Runtime/Component/LinesMesh.h"
#include "GleamRendering/Runtime/Component/PointsMesh.h"
#include "GleamECS/Runtime/Archetype.h"
#include "GleamEngine/Runtime/Entity/Archetype.h"

namespace Gleam
{
    Gleam_MakeArchetypeChild(CameraArchetype, TransformObject, WorldToLocal, ViewToClip, Camera, WorldToClip)
    Gleam_MakeArchetypeChild(PointRendererArchetype, TransformObject, PointsMesh, Renderer)
    Gleam_MakeArchetypeChild(LineRendererArchetype, TransformObject, LinesMesh, Renderer)
}