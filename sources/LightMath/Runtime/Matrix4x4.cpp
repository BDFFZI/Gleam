#include "Matrix4x4.h"

#include <format>

#include "Math.h"

const Matrix4x4 Matrix4x4::zero = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};
const Matrix4x4 Matrix4x4::identity = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

Matrix4x4 Matrix4x4::Translate(const Vector3 position)
{
    return {
        1, 0, 0, position.x,
        0, 1, 0, position.y,
        0, 0, 1, position.z,
        0, 0, 0, 1
    };
}
Matrix4x4 Matrix4x4::Rotate(const EulerAngles rotation)
{
    return Matrix3x3::Rotate(rotation);
}
Matrix4x4 Matrix4x4::Scale(const Vector3 scale)
{
    return {
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    };
}
Matrix4x4 Matrix4x4::TRS(const Vector3 position, const EulerAngles rotation, const Vector3 scale)
{
    return Translate(position) * (Matrix3x3::Rotate(rotation) * Matrix3x3::Scale(scale));
}
Matrix4x4 Matrix4x4::Ortho(const float right, const float top, const float nearClipPlane, const float farClipPlane)
{
    float depthScale = 1 / (farClipPlane - nearClipPlane);
    float depthOffset = -nearClipPlane * depthScale;
    return {
        1 / right, 0, 0, 0,
        0, 1 / top, 0, 0,
        0, 0, depthScale, depthOffset,
        0, 0, 0, 1
    };
}
Matrix4x4 Matrix4x4::Perspective(const float fieldOfView, const float aspectRatio, const float nearClipPlane, const float farClipPlane)
{
    float halfHeight = tan(fieldOfView * Deg2Rad * 0.5f) * nearClipPlane;
    float halfWidth = halfHeight * aspectRatio;

    return Ortho(halfWidth, halfHeight, nearClipPlane, farClipPlane) * Matrix4x4{
        nearClipPlane, 0, 0, 0,
        0, nearClipPlane, 0, 0,
        0, 0, nearClipPlane + farClipPlane, -nearClipPlane * farClipPlane,
        0, 0, 1, 0
    };
}

Matrix4x4::Matrix4x4(
    const float m00, const float m01, const float m02, const float m03,
    const float m10, const float m11, const float m12, const float m13,
    const float m20, const float m21, const float m22, const float m23,
    const float m30, const float m31, const float m32, const float m33)
    : m00(m00), m10(m10), m20(m20), m30(m30),
      m01(m01), m11(m11), m21(m21), m31(m31),
      m02(m02), m12(m12), m22(m22), m32(m32),
      m03(m03), m13(m13), m23(m23), m33(m33)
{
}
Matrix4x4::Matrix4x4(const Matrix3x3& matrix)
    : m00(matrix.m00), m10(matrix.m10), m20(matrix.m20), m30(0),
      m01(matrix.m01), m11(matrix.m11), m21(matrix.m21), m31(0),
      m02(matrix.m02), m12(matrix.m12), m22(matrix.m22), m32(0),
      m03(0), m13(0), m23(0), m33(1)
{
}

float Matrix4x4::GetElement(const int row, const int column) const
{
    return (&m00)[column * 4 + row];
}
Vector4 Matrix4x4::GetRow(const int row)
{
    return {
        (&m00)[row],
        (&m01)[row],
        (&m02)[row],
        (&m03)[row],
    };
}
Vector4 Matrix4x4::GetColumn(const int column)
{
    return reinterpret_cast<Vector4*>(this)[column];
}
Matrix3x3 Matrix4x4::GetCofactor(const int row, const int column, int* sign) const
{
    if (sign != nullptr)
        *sign = (row + column) % 2 == 0 ? 1 : -1;

    Matrix3x3 cofactor;

    float* p = &cofactor.m00;
    int i = 0;

    for (int columnIndex = 0; columnIndex < 4; columnIndex++)
        for (int rowIndex = 0; rowIndex < 4; rowIndex++)
            if (columnIndex != column && rowIndex != row)
            {
                p[i] = GetElement(rowIndex, columnIndex);
                i++;
            }

    // ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
    return cofactor;
}
float Matrix4x4::GetCofactorDeterminant(const int row, const int column) const
{
    int cofactorSign;
    Matrix3x3 cofactor = GetCofactor(row, column, &cofactorSign);
    return cofactor.GetDeterminant() * static_cast<float>(cofactorSign);
}
Matrix4x4 Matrix4x4::GetCofactorMatrix() const
{
    Matrix4x4 cofactorMatrix;
    for (int column = 0; column < 4; column++)
        for (int row = 0; row < 4; row++)
        {
            float cofactorDeterminant = GetCofactorDeterminant(row, column);
            cofactorMatrix.SetElement(row, column, cofactorDeterminant);
        }
    return cofactorMatrix;
}
Matrix4x4 Matrix4x4::GetTranspose()
{
    return {
        m00, m10, m20, m30,
        m01, m11, m21, m31,
        m02, m12, m22, m32,
        m03, m13, m23, m33
    };
}
Matrix4x4 Matrix4x4::GetAdjoint() const
{
    return GetCofactorMatrix().GetTranspose();
}
float Matrix4x4::GetDeterminant() const
{
    constexpr int permutations[24][5] = {
        {0, 1, 2, 3, 1},
        {0, 1, 3, 2, -1},
        {0, 2, 1, 3, -1},
        {0, 2, 3, 1, 1},
        {0, 3, 1, 2, 1},
        {0, 3, 2, 1, -1},

        {1, 0, 2, 3, -1},
        {1, 0, 3, 2, 1},
        {1, 2, 0, 3, 1},
        {1, 2, 3, 0, -1},
        {1, 3, 0, 2, -1},
        {1, 3, 2, 0, 1},

        {2, 0, 1, 3, 1},
        {2, 0, 3, 1, -1},
        {2, 1, 0, 3, -1},
        {2, 1, 3, 0, 1},
        {2, 3, 0, 1, 1},
        {2, 3, 1, 0, -1},

        {3, 0, 1, 2, -1},
        {3, 0, 2, 1, 1},
        {3, 1, 0, 2, 1},
        {3, 1, 2, 0, -1},
        {3, 2, 0, 1, -1},
        {3, 2, 1, 0, 1},
    };

    float total = 0;
    for (const auto& permutation : permutations)
    {
        float value = static_cast<float>(permutation[4]);
        for (int j = 0; j < 4; j++)
            value *= GetElement(j, permutation[j]);
        total += value;
    }
    return total;
}
Matrix4x4 Matrix4x4::GetInverse() const
{
    float determinant00 = GetCofactorDeterminant(0, 0);
    float determinant10 = GetCofactorDeterminant(1, 0);
    float determinant20 = GetCofactorDeterminant(2, 0);
    float determinant30 = GetCofactorDeterminant(3, 0);

    float determinant01 = GetCofactorDeterminant(0, 1);
    float determinant11 = GetCofactorDeterminant(1, 1);
    float determinant21 = GetCofactorDeterminant(2, 1);
    float determinant31 = GetCofactorDeterminant(3, 1);

    float determinant02 = GetCofactorDeterminant(0, 2);
    float determinant12 = GetCofactorDeterminant(1, 2);
    float determinant22 = GetCofactorDeterminant(2, 2);
    float determinant32 = GetCofactorDeterminant(3, 2);

    float determinant03 = GetCofactorDeterminant(0, 3);
    float determinant13 = GetCofactorDeterminant(1, 3);
    float determinant23 = GetCofactorDeterminant(2, 3);
    float determinant33 = GetCofactorDeterminant(3, 3);

    Matrix4x4 adjoint = {
        determinant00, determinant10, determinant20, determinant30,
        determinant01, determinant11, determinant21, determinant31,
        determinant02, determinant12, determinant22, determinant32,
        determinant03, determinant13, determinant23, determinant33,
    };

    float determinant = m00 * determinant00
        + m01 * determinant01
        + m02 * determinant02
        + m03 * determinant03;

    return adjoint / determinant;
}

void Matrix4x4::SetElement(const int row, const int column, const float value)
{
    (&m00)[column * 4 + row] = value;
}
void Matrix4x4::SetRow(const int row, const Vector4 vector)
{
    (&m00)[row] = vector.x;
    (&m01)[row] = vector.y;
    (&m02)[row] = vector.z;
    (&m03)[row] = vector.w;
}
void Matrix4x4::SetColumn(const int column, const Vector4 vector)
{
    reinterpret_cast<Vector4*>(this)[column] = vector;
}

std::string to_string(const Matrix4x4& matrix)
{
    return std::format("|{:f} {:f} {:f} {:f}|\n|{:f} {:f} {:f} {:f}|\n|{:f} {:f} {:f} {:f}|\n|{:f} {:f} {:f} {:f}|",
                       matrix.m00, matrix.m01, matrix.m02, matrix.m03,
                       matrix.m10, matrix.m11, matrix.m12, matrix.m13,
                       matrix.m20, matrix.m21, matrix.m22, matrix.m23,
                       matrix.m30, matrix.m31, matrix.m32, matrix.m33);
}
Matrix4x4 operator*(const Matrix4x4& left, const Matrix4x4& right)
{
    Matrix4x4 matrix;

    matrix.m00 = left.m00 * right.m00 + left.m01 * right.m10 + left.m02 * right.m20 + left.m03 * right.m30;
    matrix.m01 = left.m00 * right.m01 + left.m01 * right.m11 + left.m02 * right.m21 + left.m03 * right.m31;
    matrix.m02 = left.m00 * right.m02 + left.m01 * right.m12 + left.m02 * right.m22 + left.m03 * right.m32;
    matrix.m03 = left.m00 * right.m03 + left.m01 * right.m13 + left.m02 * right.m23 + left.m03 * right.m33;

    matrix.m10 = left.m10 * right.m00 + left.m11 * right.m10 + left.m12 * right.m20 + left.m13 * right.m30;
    matrix.m11 = left.m10 * right.m01 + left.m11 * right.m11 + left.m12 * right.m21 + left.m13 * right.m31;
    matrix.m12 = left.m10 * right.m02 + left.m11 * right.m12 + left.m12 * right.m22 + left.m13 * right.m32;
    matrix.m13 = left.m10 * right.m03 + left.m11 * right.m13 + left.m12 * right.m23 + left.m13 * right.m33;

    matrix.m20 = left.m20 * right.m00 + left.m21 * right.m10 + left.m22 * right.m20 + left.m23 * right.m30;
    matrix.m21 = left.m20 * right.m01 + left.m21 * right.m11 + left.m22 * right.m21 + left.m23 * right.m31;
    matrix.m22 = left.m20 * right.m02 + left.m21 * right.m12 + left.m22 * right.m22 + left.m23 * right.m32;
    matrix.m23 = left.m20 * right.m03 + left.m21 * right.m13 + left.m22 * right.m23 + left.m23 * right.m33;

    matrix.m30 = left.m30 * right.m00 + left.m31 * right.m10 + left.m32 * right.m20 + left.m33 * right.m30;
    matrix.m31 = left.m30 * right.m01 + left.m31 * right.m11 + left.m32 * right.m21 + left.m33 * right.m31;
    matrix.m32 = left.m30 * right.m02 + left.m31 * right.m12 + left.m32 * right.m22 + left.m33 * right.m32;
    matrix.m33 = left.m30 * right.m03 + left.m31 * right.m13 + left.m32 * right.m23 + left.m33 * right.m33;

    return matrix;
}
Vector4 operator*(const Matrix4x4& left, const Vector4 right)
{
    return {
        left.m00 * right.x + left.m01 * right.y + left.m02 * right.z + left.m03 * right.w,
        left.m10 * right.x + left.m11 * right.y + left.m12 * right.z + left.m13 * right.w,
        left.m20 * right.x + left.m21 * right.y + left.m22 * right.z + left.m23 * right.w,
        left.m30 * right.x + left.m31 * right.y + left.m32 * right.z + left.m33 * right.w,
    };
}
Matrix4x4 operator*(const Matrix4x4& left, const float right)
{
    return {
        left.m00 * right, left.m01 * right, left.m02 * right, left.m03 * right,
        left.m10 * right, left.m11 * right, left.m12 * right, left.m13 * right,
        left.m20 * right, left.m21 * right, left.m22 * right, left.m23 * right,
        left.m30 * right, left.m31 * right, left.m32 * right, left.m33 * right
    };
}
Matrix4x4 operator/(const Matrix4x4& left, const float right)
{
    return left * (1 / right);
}
bool operator==(const Matrix4x4& left, const Matrix4x4& right)
{
    const float* leftValue = &left.m00;
    const float* rightValue = &right.m00;
    for (int i = 0; i < 16; i++)
    {
        float diff = leftValue[i] - rightValue[i];
        if (diff * diff >= SqrEPSILON)
            return false;
    }

    return true;
}
