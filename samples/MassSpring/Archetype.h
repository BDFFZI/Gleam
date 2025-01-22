#pragma once
#include "Public/Component.h"
#include "Rendering/RenderingComponent.h"
#include "Physics/PhysicsComponent.h"

Light_MakeArchetype(MassPointArchetype, Light::Point, Light::Renderer, Light::MassPointPhysics)
Light_MakeArchetype(SpringArchetype, Light::Segment, Light::Renderer, Light::SpringPhysics)
Light_MakeArchetype(LineArchetype, Light::Segment, Light::Renderer)
