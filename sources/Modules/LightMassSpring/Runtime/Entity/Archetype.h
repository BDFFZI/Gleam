#pragma once
#include "LightECS/Runtime/Archetype.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightMassSpring/Runtime/Component/MassPointPhysics.h"
#include "LightMassSpring/Runtime/Component/SpringPhysics.h"
#include "LightMath/Runtime/Geometry/Point.h"

#ifdef LightRenderingRuntime
#include "LightRendering/Runtime/Entity/Archetype.h"
#include "LightMassSpring/Runtime/Component/MassSpringMesh.h"
#endif

namespace Light
{
    Light_MakeArchetype(MassPointArchetype, Point, MassPointPhysics);
    Light_MakeArchetype(SpringArchetype, SpringPhysics);
    
#ifdef LightRenderingRuntime
    Light_MakeArchetypeChild(MassPointRendererArchetype, PointRendererArchetype, MassSpringMesh);
    Light_MakeArchetypeChild(SpringRendererArchetype, LineRendererArchetype, MassSpringMesh);
#endif
}