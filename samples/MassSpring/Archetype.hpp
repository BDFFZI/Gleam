#pragma once
#include "Public/Component.hpp"
#include "Rendering/RenderingComponent.hpp"
#include "Physics/PhysicsComponent.hpp"

MakeArchetype(MassPointArchetype, Light::Point, Light::Renderer, Light::MassPointPhysics)
MakeArchetype(SpringArchetype, Light::Line, Light::Renderer, Light::SpringPhysics)
MakeArchetype(LineArchetype, Light::Line, Light::Renderer)
