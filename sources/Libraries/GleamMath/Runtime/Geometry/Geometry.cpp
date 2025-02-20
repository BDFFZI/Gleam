#include "Geometry.h"

#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

namespace Gleam
{
    bool Geometry::Contains(const Rectangle& rectangle, const float2 point)
    {
        return all(point > rectangle.min) && all(point < rectangle.max);
    }
    bool Geometry::Contains(const Cuboid& cuboid, const float3 point)
    {
        return all(point > cuboid.min) && all(point < cuboid.max);
    }
    bool Geometry::Contains(const Sphere& sphere, const float3 point)
    {
        float3 localPoint = point - sphere.center;
        return lengthsq(localPoint) < sphere.radius * sphere.radius;
    }
    float3 Geometry::Extrudes(const Cuboid& cuboid, float3 point)
    {
        float3 halfSize = cuboid.GetSize() * 0.5f;
        float3 localPoint = (point - cuboid.GetCenter()) / halfSize;
        float3 localLength = abs(localPoint);

        if (localLength.x > localLength.y)
        {
            if (localLength.x > localLength.z)
                return {localPoint.x > 0 ? cuboid.max.x : cuboid.min.x, point.y, point.z};
            else
                return {point.x, point.y, localPoint.z > 0 ? cuboid.max.z : cuboid.min.z};
        }
        else
        {
            if (localLength.y > localLength.z)
                return {point.x, localPoint.y > 0 ? cuboid.max.y : cuboid.min.y, point.z};
            else
                return {point.x, point.y, localPoint.z > 0 ? cuboid.max.z : cuboid.min.z};
        }
    }
    float3 Geometry::Extrudes(const Sphere& sphere, float3 point)
    {
        float3 localPoint = point - sphere.center;
        float3 direction = normalize(localPoint);
        return sphere.center + direction * sphere.radius;
    }
    float3 Geometry::Intersects(const Plane& plane, const Line& line)
    {
        float t = dot(plane.normal, plane.position - line.position) / dot(plane.normal, line.direction);
        return line.direction * t + line.position;
    }

    std::string Geometry::to_string(const Rectangle& rectangle)
    {
        return Gleam::to_string(rectangle.min) + Gleam::to_string(rectangle.max);
    }
}