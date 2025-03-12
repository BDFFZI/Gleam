#pragma once
#include "GleamECS/Runtime/Archetype.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMath/Runtime/Geometry/2D/Rectangle.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"
#include "GleamMath/Runtime/Geometry/3D/Cuboid.h"
#include "GleamMath/Runtime/Geometry/3D/Line.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"

namespace Gleam
{
    void DrawRectangle(Entity entity, const Rectangle& rectangle, const float4& color = float4::White());
    void DrawCuboid(Entity entity, const Cuboid& cuboid, const float4& color = float4::White());
    void DrawSphere(Entity entity, const Sphere& sphere, const float4& color = float4::White());

    void InspectorWindowUI_Point(Point& point);
    void InspectorWindowUI_Segment(Segment& segment);
    void InspectorWindowUI_Cuboid(Cuboid& cuboid);
    void InspectorWindowUI_Rectangle(Rectangle& rectangle);
    void InspectorWindowUI_Sphere(Sphere& sphere);

    inline static Entity SceneWindowUI_Entity_Target = Entity::Null;
    void SceneWindowUI_Entity(Entity entity);
    void SceneWindowUI_LocalToWorld(LocalToWorld& localToWorld);
}