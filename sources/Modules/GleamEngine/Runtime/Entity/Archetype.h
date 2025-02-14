#pragma once
#include "GleamECS/Runtime/Archetype.h"
#include "GleamEngine/Runtime/Component/Transform.h"

namespace Gleam
{
    Gleam_MakeArchetype(TransformObject, LocalTransform, LocalToWorld)
}