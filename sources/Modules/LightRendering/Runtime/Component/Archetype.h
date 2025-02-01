#pragma once
#include "Camera.h"
#include "LinesMesh.h"
#include "PointsMesh.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightECS/Runtime/Archetype.h"

namespace Light
{
    Light_MakeArchetype(CameraArchetype, LocalTransform, LocalToWorld, Camera, CameraTransform)
    Light_MakeArchetype(PointsArchetype, LocalTransform, LocalToWorld, PointsMesh, Renderer)
    Light_MakeArchetype(LinesArchetype, LocalTransform, LocalToWorld, LinesMesh, Renderer)
}