#pragma once
#include "Vector4.h"

struct Matrix4x4
{
    static float Determinant(Matrix4x4 matrix);
    static Matrix4x4 Transpose(Matrix4x4 matrix);
    static Matrix4x4 Inverse(Matrix4x4 matrix);
    static Vector4 Multiply(Matrix4x4 matrix, Vector4 vector);

    //为了与图像接口兼容故采用按列存储
    float m00, m10, m20, m30;
    float m01, m11, m21, m31;
    float m02, m12, m22, m32;
    float m03, m13, m23, m33;
};
