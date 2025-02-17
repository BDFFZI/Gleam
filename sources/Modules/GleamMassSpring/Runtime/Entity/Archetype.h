#pragma once
#include "GleamECS/Runtime/Archetype.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamEngine/Runtime/Entity/Archetype.h"
#include "GleamMassSpring/Runtime/Component/Collider.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMassSpring/Runtime/Component/SpringPhysics.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"
#include "GleamMath/Runtime/Geometry/Solid/Cuboid.h"
#include "GleamMath/Runtime/Geometry/Solid/Sphere.h"

#ifdef GleamRenderingRuntime
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamMassSpring/Runtime/Component/MassSpringMesh.h"
#endif

namespace Gleam
{
    Gleam_MakeArchetype(MassPointArchetype, Point, MassPointPhysics);
    Gleam_MakeArchetype(SpringArchetype, SpringPhysics);
    Gleam_MakeArchetypeChild(CuboidCollider, TransformObject, WorldToLocal, Cuboid, Collider)
    Gleam_MakeArchetypeChild(SphereCollider, TransformObject, WorldToLocal, Sphere, Collider)

#ifdef GleamRenderingRuntime
    Gleam_MakeArchetypeChild(MassPointRendererArchetype, PointRendererArchetype, MassSpringMesh);
    Gleam_MakeArchetypeChild(SpringRendererArchetype, LineRendererArchetype, MassSpringMesh);
#endif
}