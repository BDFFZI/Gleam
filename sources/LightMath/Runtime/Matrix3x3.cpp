#include "Matrix3x3.h"

#include <format>

#include "Math.h"

Matrix3x3 Matrix3x3::Rotate(const EulerAngles rotation)
{
    EulerAngles radians = rotation * Deg2Rad;
    
    float cosX = cos(radians.x);
    float sinX = sin(radians.x);
    Matrix3x3 xRotate = {
        1, 0, 0,
        0, cosX, -sinX,
        0, sinX, cosX,
    };

    float cosY = cos(radians.y);
    float sinY = sin(radians.y);
    Matrix3x3 yRotate = {
        cosY, 0, sinY,
        0, 1, 0,
        -sinY, 0, cosY,
    };

    float cosZ = cos(radians.z);
    float sinZ = sin(radians.z);
    Matrix3x3 zRotate = {
        cosZ, -sinZ, 0,
        sinZ, cosZ, 0,
        0, 0, 1,
    };

    return yRotate * xRotate * zRotate;
}
Matrix3x3 Matrix3x3::Scale(const Vector3 scale)
{
    return {
        scale.x, 0, 0,
        0, scale.y, 0,
        0, 0, scale.z,
    };
}
Matrix3x3::Matrix3x3(
    const float m00, const float m01, const float m02,
    const float m10, const float m11, const float m12,
    const float m20, const float m21, const float m22)
    : m00(m00), m10(m10), m20(m20),
      m01(m01), m11(m11), m21(m21),
      m02(m02), m12(m12), m22(m22)
{
}

float Matrix3x3::GetDeterminant() const
{
    return m00 * (m11 * m22 - m12 * m21)
        - m01 * (m10 * m22 - m12 * m20)
        + m02 * (m10 * m21 - m11 * m20);
}
std::string to_string(const Matrix3x3& matrix)
{
    return std::format("|{:f} {:f} {:f}|\n|{:f} {:f} {:f}|\n|{:f} {:f} {:f}|",
                       matrix.m00, matrix.m01, matrix.m02,
                       matrix.m10, matrix.m11, matrix.m12,
                       matrix.m20, matrix.m21, matrix.m22);
}

Matrix3x3 operator*(const Matrix3x3& left, const Matrix3x3& right)
{
    Matrix3x3 matrix;

    matrix.m00 = left.m00 * right.m00 + left.m01 * right.m10 + left.m02 * right.m20;
    matrix.m01 = left.m00 * right.m01 + left.m01 * right.m11 + left.m02 * right.m21;
    matrix.m02 = left.m00 * right.m02 + left.m01 * right.m12 + left.m02 * right.m22;

    matrix.m10 = left.m10 * right.m00 + left.m11 * right.m10 + left.m12 * right.m20;
    matrix.m11 = left.m10 * right.m01 + left.m11 * right.m11 + left.m12 * right.m21;
    matrix.m12 = left.m10 * right.m02 + left.m11 * right.m12 + left.m12 * right.m22;

    matrix.m20 = left.m20 * right.m00 + left.m21 * right.m10 + left.m22 * right.m20;
    matrix.m21 = left.m20 * right.m01 + left.m21 * right.m11 + left.m22 * right.m21;
    matrix.m22 = left.m20 * right.m02 + left.m21 * right.m12 + left.m22 * right.m22;

    return matrix;
}
