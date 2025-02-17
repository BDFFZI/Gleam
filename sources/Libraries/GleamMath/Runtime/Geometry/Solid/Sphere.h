#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    class Sphere
    {
    public:
        Sphere();
        Sphere(float3 center, float radius);

        float3 GetCenter() const;
        float GetRadius() const;

        bool Contains(float3 point) const;
        float3 Extrudes(float3 point) const;

    private:
#ifdef GleamReflectionRuntime
        Gleam_MakeType_Friend
#endif
        
        float3 center;
        float radius;
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Sphere,"E81919B5-E944-4552-B874-453487FE3D62")
    {
        Gleam_MakeType_AddField(center);
        Gleam_MakeType_AddField(radius);
    }
#endif
}