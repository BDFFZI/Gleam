#pragma once
#include "Camera.h"
#include "LinesMesh.h"
#include "PointsMesh.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightECS/Runtime/Archetype.h"

namespace Light
{
    Light_MakeArchetype(CameraArchetype, Transform, Camera)
    Light_MakeArchetype(PointsArchetype, Transform, PointsMesh, Renderer)
    Light_MakeArchetype(LinesArchetype, Transform, LinesMesh, Renderer)
}