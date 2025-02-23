#pragma once
#include "GleamECS/Runtime/Archetype.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamEngine/Runtime/Entity/Archetype.h"
#include "GleamMassSpring/Runtime/Component/Collider.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMassSpring/Runtime/Component/Spring.h"
#include "GleamMath/Runtime/Geometry/2D/Rectangle.h"
#include "GleamMath/Runtime/Geometry/3D/Cuboid.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"

namespace Gleam
{
    Gleam_MakeArchetype(ParticleArchetype, Particle);
    Gleam_MakeArchetype(SpringArchetype, Spring);
    Gleam_MakeArchetypeChild(RectangleCollider, TransformObject, WorldToLocal, Rectangle, Collider)
    Gleam_MakeArchetypeChild(CuboidCollider, TransformObject, WorldToLocal, Cuboid, Collider)
    Gleam_MakeArchetypeChild(SphereCollider, TransformObject, WorldToLocal, Sphere, Collider)
}