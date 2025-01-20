#pragma once
#include "Public/Component.hpp"
#include "Rendering/RenderingComponent.hpp"
#include "Physics/PhysicsComponent.hpp"

Light_MakeArchetype(MassPointArchetype, Light::Point, Light::Renderer, Light::MassPointPhysics)
Light_MakeArchetype(SpringArchetype, Light::Segment, Light::Renderer, Light::SpringPhysics)
Light_MakeArchetype(LineArchetype, Light::Segment, Light::Renderer)
