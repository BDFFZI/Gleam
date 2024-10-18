#pragma once
#include "EulerAngles.h"
#include "Vector3.h"

struct Matrix3x3
{
    static Matrix3x3 Rotate(EulerAngles rotation);
    static Matrix3x3 Scale(Vector3 scale);

    //为了与图像接口兼容故采用按列存储
    float m00, m10, m20;
    float m01, m11, m21;
    float m02, m12, m22;

    Matrix3x3() = default;
    Matrix3x3(float m00, float m01, float m02,
              float m10, float m11, float m12,
              float m20, float m21, float m22);

    float GetDeterminant() const;
};

std::string to_string(const Matrix3x3& matrix);
Matrix3x3 operator*(const Matrix3x3& left, const Matrix3x3& right);
