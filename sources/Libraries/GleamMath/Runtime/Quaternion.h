#pragma once
#include "LinearAlgebra/Matrix.h"
#include "LinearAlgebra/Vector.h"

#ifdef GleamReflectionRuntime
#include "GleamReflection/Runtime/Type.h"
#endif

namespace Gleam
{
    struct Quaternion
    {
        static consteval Quaternion Identity() { return {0, 0, 0, 1}; }
        static Quaternion Euler(float3 eulerAngles);
        static Quaternion AngleAxis(float3 axis, float angle);
        static Quaternion Matrix(const float3x3& rotation);

        float x, y, z, w;

        constexpr Quaternion() = default;
        constexpr Quaternion(const float x, const float y, const float z, const float w)
            : x(x), y(y), z(z), w(w)
        {
        }
        /**
        * 通过向量构建纯四元数
        * @param vector 
        */
        constexpr Quaternion(const float3 vector)
            : x(vector.x), y(vector.y), z(vector.z), w(0)
        {
        }
        /**
          * 通过分开表示实部和虚部来构建四元数
          * @param w 
          * @param xyz 
          */
        constexpr Quaternion(const float w, const float3 xyz)
            : x(xyz.x), y(xyz.y), z(xyz.z), w(w)
        {
        }

        Quaternion operator*(Quaternion other) const;
        bool operator==(Quaternion other) const;
        bool operator!=(Quaternion other) const;

        float3 GetImaginaryPart() const;
        Quaternion GetConjugate() const;
        float GetRotationAngle() const;
        float3 GetRotationAxis() const;

        float3 Rotate(float3 vector) const;

        float3x3 ToRotationMatrix() const;
        float3 ToEulerAngles() const;
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Quaternion, "DBBF47A3-169D-464C-9664-D403454B2480")
    {
        Gleam_MakeType_AddField(x);
        Gleam_MakeType_AddField(y);
        Gleam_MakeType_AddField(z);
        Gleam_MakeType_AddField(w);
    }
#endif
}