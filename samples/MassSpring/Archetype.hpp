#pragma once
#include "Public/Component.hpp"
#include "Rendering/RenderingComponent.hpp"
#include "Physics/PhysicsComponent.hpp"

MakeArchetype(MassPointArchetype, Light::Point, Light::Renderer, Light::MassPointPhysics)
MakeArchetype(SpringArchetype, Light::Segment, Light::Renderer, Light::SpringPhysics)
MakeArchetype(LineArchetype, Light::Segment, Light::Renderer)
