#pragma once
#include "LightECS/Runtime/Archetype.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightMassSpring/Runtime/Component/MassPointPhysics.h"
#include "LightMassSpring/Runtime/Component/MassSpringMesh.h"
#include "LightMassSpring/Runtime/Component/SpringPhysics.h"
#include "LightMath/Runtime/Geometry/Point.h"
#include "LightRendering/Runtime/Entity/Archetype.h"

namespace Light
{
    Light_MakeArchetype(MassPointArchetype, Point, MassPointPhysics);
    Light_MakeArchetype(SpringArchetype, SpringPhysics);
    Light_MakeArchetypeChild(MassPointRendererArchetype, PointRendererArchetype, MassSpringMesh);
    Light_MakeArchetypeChild(SpringRendererArchetype, LineRendererArchetype, MassSpringMesh);
}