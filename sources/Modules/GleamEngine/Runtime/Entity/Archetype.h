#pragma once
#include "GleamECS/Runtime/Entity/Archetype.h"
#include "Transform.h"

namespace Gleam
{
    Gleam_MakeArchetype(TransformObject, LocalTransform, LocalToWorld)
}