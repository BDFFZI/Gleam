#pragma once
#include "2D/Rectangle.h"
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "3D/Cuboid.h"
#include "3D/Plane.h"
#include "3D/Sphere.h"

namespace Gleam
{
    class Geometry
    {
    public:
        static bool Contains(const Rectangle& rectangle, float2 point, float padding = std::numeric_limits<float>::epsilon());
        static bool Contains(const Rectangle& rectangle, float3 point, float padding = std::numeric_limits<float>::epsilon());
        static bool Contains(const Cuboid& cuboid, float3 point, float padding = std::numeric_limits<float>::epsilon());
        static bool Contains(const Sphere& sphere, float3 point, float padding = std::numeric_limits<float>::epsilon());

        static float2 Extrudes(const Rectangle& rectangle, float2 point);
        static float3 Extrudes(const Rectangle& rectangle, float3 point);
        static float3 Extrudes(const Cuboid& cuboid, float3 point);
        static float3 Extrudes(const Sphere& sphere, float3 point);
        static float3 Intersects(const Plane& plane, const Line& line);

        static std::string to_string(const Rectangle& rectangle);
    };
}