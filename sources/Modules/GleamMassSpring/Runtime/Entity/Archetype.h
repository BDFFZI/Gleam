#pragma once
#include "GleamECS/Runtime/Archetype.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMassSpring/Runtime/Component/SpringPhysics.h"
#include "GleamMath/Runtime/Geometry/Point.h"

#ifdef GleamRenderingRuntime
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamMassSpring/Runtime/Component/MassSpringMesh.h"
#endif

namespace Gleam
{
    Gleam_MakeArchetype(MassPointArchetype, Point, MassPointPhysics);
    Gleam_MakeArchetype(SpringArchetype, SpringPhysics);
    
#ifdef GleamRenderingRuntime
    Gleam_MakeArchetypeChild(MassPointRendererArchetype, PointRendererArchetype, MassSpringMesh);
    Gleam_MakeArchetypeChild(SpringRendererArchetype, LineRendererArchetype, MassSpringMesh);
#endif
}