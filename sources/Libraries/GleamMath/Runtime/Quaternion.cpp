#include "Quaternion.h"

#include "LinearAlgebra/MatrixMath.h"
#include "LinearAlgebra/VectorMath.h"

namespace Gleam
{
    Quaternion Quaternion::Euler(const float3 eulerAngles)
    {
        float3 rad = radians(eulerAngles) / 2;
        const Quaternion z = {0, 0, sin(rad.z), cos(rad.z)};
        const Quaternion x = {sin(rad.x), 0, 0, cos(rad.x)};
        const Quaternion y = {0, sin(rad.y), 0, cos(rad.y)};
        return y * x * z;
    }
    Quaternion Quaternion::AngleAxis(const float3 axis, const float angle)
    {
        float rad = radians(angle) / 2;
        return {cos(rad), sin(rad) * axis};
    }
    Quaternion Quaternion::Matrix(const float3x3& rotation)
    {
        //TODO 替换成真正的矩阵转欧拉角
        return Euler(DecomposeRotation(rotation));
    }

    Quaternion Quaternion::operator*(const Quaternion other) const
    {
        const float3 u = GetImaginaryPart();
        const float3 v = other.GetImaginaryPart();
        return {w * other.w - dot(u, v), w * v + other.w * u + cross(u, v)};
    }
    bool Quaternion::operator==(const Quaternion other) const
    {
        return equal(x, other.x) && equal(y, other.y) && equal(z, other.z) && equal(w, other.w);
    }
    bool Quaternion::operator!=(const Quaternion other) const
    {
        return !(*this == other);
    }

    float3 Quaternion::GetImaginaryPart() const
    {
        return {x, y, z};
    }
    Quaternion Quaternion::GetConjugate() const
    {
        return {-x, -y, -z, w};
    }
    float Quaternion::GetRotationAngle() const
    {
        return degrees(acos(w)) * 2;
    }
    float3 Quaternion::GetRotationAxis() const
    {
        const float factor = sin(acos(w));
        if (equal(factor, 0.0f))
            return 0;
        return float3{x, y, z} / factor;
    }

    float3 Quaternion::Rotate(const float3 vector) const
    {
        return (*this * Quaternion(vector) * GetConjugate()).GetImaginaryPart();
    }

    float3x3 Quaternion::ToRotationMatrix() const
    {
        const float a = w;
        const float b = x;
        const float c = y;
        const float d = z;
        return float3x3{
            1 - 2 * c * c - 2 * d * d, 2 * b * c - 2 * a * d, 2 * a * c + 2 * b * d,
            2 * b * c + 2 * a * d, 1 - 2 * b * b - 2 * d * d, 2 * c * d - 2 * a * b,
            2 * b * d - 2 * a * c, 2 * a * b + 2 * c * d, 1 - 2 * b * b - 2 * c * c,
        };
    }
    float3 Quaternion::ToEulerAngles() const
    {
        return DecomposeRotation(ToRotationMatrix());
    }
}