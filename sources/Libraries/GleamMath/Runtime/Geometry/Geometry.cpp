#include "Geometry.h"

#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

namespace Gleam
{
    bool Geometry::Contains(const Rectangle& rectangle, const float2 point, const float padding)
    {
        return all(point - rectangle.min > padding) && all(rectangle.max - point > padding);
    }
    bool Geometry::Contains(const Rectangle& rectangle, const float3 point, const float padding)
    {
        return Contains(rectangle, point.xy, padding);
    }
    bool Geometry::Contains(const Cuboid& cuboid, const float3 point, const float padding)
    {
        return all(point - cuboid.min > padding) && all(cuboid.max - point > padding);
    }
    bool Geometry::Contains(const Sphere& sphere, const float3 point, const float padding)
    {
        float3 localPoint = point - sphere.center;
        return lengthsq(localPoint) < (sphere.radius - padding) * (sphere.radius - padding);
    }

    float2 Geometry::Extrudes(const Rectangle& rectangle, float2 point)
    {
        float2 distanceToMin = point - rectangle.min;
        float2 distanceToMax = rectangle.max - point;
        float2 distanceToFrame = min(distanceToMin, distanceToMax);

        if (distanceToFrame.x < distanceToFrame.y)
            return {distanceToMin.x < distanceToMax.x ? rectangle.min.x : rectangle.max.x, point.y};
        else
            return {point.x, distanceToMin.y < distanceToMax.y ? rectangle.min.y : rectangle.max.y};
    }
    float3 Geometry::Extrudes(const Rectangle& rectangle, float3 point)
    {
        return {Extrudes(rectangle, point.xy), point.z};
    }
    float3 Geometry::Extrudes(const Cuboid& cuboid, float3 point)
    {
        float3 distanceToMin = point - cuboid.min;
        float3 distanceToMax = cuboid.max - point;
        float3 distanceToFrame = min(distanceToMin, distanceToMax);

        if (distanceToFrame.x < distanceToFrame.y)
        {
            if (distanceToFrame.x < distanceToFrame.z)
                return {distanceToMin.x < distanceToMax.x ? cuboid.min.x : cuboid.max.x, point.y, point.z};
            else
                return {point.x, point.y, distanceToMin.z < distanceToMax.z ? cuboid.min.z : cuboid.max.z};
        }
        else
        {
            if (distanceToFrame.y < distanceToFrame.z)
                return {point.x, distanceToMin.y < distanceToMax.y ? cuboid.min.y : cuboid.max.y, point.z};
            else
                return {point.x, point.y, distanceToMin.z < distanceToMax.z ? cuboid.min.z : cuboid.max.z};
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