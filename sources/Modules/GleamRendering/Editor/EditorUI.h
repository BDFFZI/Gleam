#pragma once
#include "GleamECS/Runtime/Entity.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"
#include "GleamMath/Runtime/Geometry/Solid/Cuboid.h"
#include "GleamMath/Runtime/Geometry/Solid/Segment.h"
#include "GleamMath/Runtime/Geometry/Solid/Sphere.h"

namespace Gleam
{
    void DrawCuboid(Entity entity, Cuboid& cuboid, const float4& color = float4::White());
    void DrawSphere(Entity entity, Sphere& sphere, const float4& color = float4::White());

    void SceneGUI_Entity(Entity entity);
    void SceneGUI_LocalToWorld(LocalToWorld& localToWorld);
    void SceneGUI_Point(Point& point);

    void InspectorGUI_Point(Point& point);
    void InspectorGUI_Segment(Segment& segment);
    void InspectorGUI_Cuboid(Cuboid& cuboid);
    void InspectorGUI_Sphere(Sphere& sphere);
}