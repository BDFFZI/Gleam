#pragma once
#include "GleamECS/Runtime/Entity.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"
#include "GleamMath/Runtime/Geometry/3D/Cuboid.h"
#include "GleamMath/Runtime/Geometry/3D/Line.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"

namespace Gleam
{
    void DrawCuboid(Entity entity, Cuboid& cuboid, const float4& color = float4::White());
    void DrawSphere(Entity entity, Sphere& sphere, const float4& color = float4::White());

    void SceneUI_Entity(Entity entity);
    void SceneUI_LocalToWorld(LocalToWorld& localToWorld);
    void SceneUI_Point(Point& point);

    void InspectorUI_Point(Point& point);
    void InspectorUI_Segment(Segment& segment);
    void InspectorUI_Cuboid(Cuboid& cuboid);
    void InspectorUI_Sphere(Sphere& sphere);
}