#pragma once
#include "GleamECS/Runtime/Entity/Archetype.h"
#include "Transform.h"
#include "Time.h"

namespace Gleam
{
    Gleam_MakeArchetype(TransformObject, LocalTransform, LocalToWorld)
    Gleam_MakeArchetype(TimeData, Time)
}