#pragma once
#include "LightRendering/Runtime/Entity/Archetype.h"
#include "Physics/PhysicsComponent.h"
#include "LightMath/Runtime/Geometry/Point.h"

Light_MakeArchetype(MassPointArchetype, Light::Point, Light::MassPointPhysics);
Light_MakeArchetype(SpringArchetype, Light::Segment, Light::SpringPhysics)
Light_MakeArchetype(LineArchetype, Light::Segment)