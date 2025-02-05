#pragma once
#include "LightECS/Runtime/Archetype.h"
#include "LightMassSpring/Runtime/Component/MassPointPhysics.h"
#include "LightMassSpring/Runtime/Component/SpringPhysics.h"
#include "LightMath/Runtime/Geometry/Point.h"

namespace Light
{
    Light_MakeArchetype(MassPointArchetype, Point, MassPointPhysics);
    Light_MakeArchetype(SpringArchetype, SpringPhysics);
}