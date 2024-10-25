#pragma once
#include "Vector_Math.hpp"
#include "Matrix.hpp"

/**
* 创建一个旋转矩阵
 * @param degree 基于欧拉角的三轴旋转角度
 * @return 
 */
template <class Type>
constexpr matrix<Type, 3, 3> rotation(vector<Type, 3> degree)
{
    float3 rad = radians(degree);

    float cosX = cos(rad.x);
    float sinX = sin(rad.x);
    float3x3 xRotate = {
        1, 0, 0,
        0, cosX, -sinX,
        0, sinX, cosX,
    };

    float cosY = cos(rad.y);
    float sinY = sin(rad.y);
    float3x3 yRotate = {
        cosY, 0, sinY,
        0, 1, 0,
        -sinY, 0, cosY,
    };

    float cosZ = cos(rad.z);
    float sinZ = sin(rad.z);
    float3x3 zRotate = {
        cosZ, -sinZ, 0,
        sinZ, cosZ, 0,
        0, 0, 1,
    };

    return yRotate * xRotate * zRotate;
}

/**
 * 创建一个缩放矩阵
 * @param scale 三维空间的各轴缩放
 * @return 
 */
template <class Type>
constexpr matrix<Type, 3, 3> scale(vector<Type, 3> scale)
{
    return {
        scale.x, 0, 0,
        0, scale.y, 0,
        0, 0, scale.z,
    };
}

template <class Type>
constexpr matrix<Type, 4, 4> translation(vector<Type, 3> position)
{
    return {
        1, 0, 0, position.x,
        0, 1, 0, position.y,
        0, 0, 1, position.z,
        0, 0, 0, 1
    };
}

template <class Type>
constexpr matrix<Type, 4, 4> trs(vector<Type, 3> position, vector<Type, 3> rotation, vector<Type, 3> scale)
{
    return translation(position) * (rotation(rotation) * scale(scale));
}

/**
 * 创建一个正交投影矩阵，其剪辑空间遵从从右到左、从下到上都为[-1,1]，深度从近到远为[0-1]的约定。
 * @tparam Type 
 * @param right 视野范围半宽
 * @param top 视野范围半高
 * @param nearClipPlane 视野范围近平面
 * @param farClipPlane 视野范围远平面
 * @return 
 */
template <class Type>
constexpr matrix<Type, 4, 4> ortho(const float right, const float top, const float nearClipPlane, const float farClipPlane)
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

/**
 * 创建一个透视投影矩阵，其剪辑空间遵从从右到左、从下到上都为[-1,1]，深度从近到远为[0-1]的约定。
 * @tparam Type 
 * @param fieldOfView 视锥体上下夹角
 * @param aspectRatio 视野范围宽高比（宽度/高度）
 * @param nearClipPlane 视野范围近平面
 * @param farClipPlane 视野范围远平面
 * @return 
 */
template <class Type>
constexpr matrix<Type, 4, 4> perspective(const float fieldOfView, const float aspectRatio, const float nearClipPlane, const float farClipPlane)
{
    float halfHeight = tan(radians(fieldOfView) * 0.5f) * nearClipPlane;
    float halfWidth = halfHeight * aspectRatio;

    return ortho<Type>(halfWidth, halfHeight, nearClipPlane, farClipPlane) * matrix<Type, 4, 4>{
        nearClipPlane, 0, 0, 0,
        0, nearClipPlane, 0, 0,
        0, 0, nearClipPlane + farClipPlane, -nearClipPlane * farClipPlane,
        0, 0, 1, 0
    };
}

template <class Type, int Row, int Column>
constexpr matrix<Type, Row, Column> transpose(const matrix<Type, Row, Column>& m)
{
    matrix<Type, Row, Column> result;
    for (int row = 0; row < Row; row++)
        for (int column = 0; column < Column; column++)
            result[row][column] = m[column][row];
    return result;
}

template <class Type>
constexpr Type determinant(const matrix<Type, 3, 3>& m)
{
    return
        m._m00 * (m._m11 * m._m22 - m._m12 * m._m21) -
        m._m01 * (m._m10 * m._m22 - m._m12 * m._m20) +
        m._m02 * (m._m10 * m._m21 - m._m11 * m._m20);
}

template <class Type>
constexpr Type determinant(const matrix<Type, 4, 4>& m)
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

    Type total = 0;
    for (const auto& permutation : permutations)
    {
        Type value = static_cast<Type>(permutation[4]);
        for (int j = 0; j < 4; j++)
            value *= m[j][permutation[j]];
        total += value;
    }
    return total;
}

template <class Type, int Row, int Column>
constexpr matrix<Type, Row - 1, Column - 1> cofactor(const matrix<Type, Row, Column>& m, const int row, const int column, int* sign)
{
    if (sign != nullptr)
        *sign = (row + column) % 2 == 0 ? 1 : -1;

    matrix<Type, Row - 1, Column - 1> cofactor;

    int i = 0;
    for (int columnIndex = 0; columnIndex < Column; columnIndex++)
        for (int rowIndex = 0; rowIndex < Row; rowIndex++)
            if (columnIndex != column && rowIndex != row)
            {
                cofactor.data[i] = m[rowIndex][columnIndex];
                i++;
            }

    return cofactor;
}

template <class Type, int Row, int Column>
Type cofactor_determinant(const matrix<Type, Row, Column>& m, const int row, const int column)
{
    int cofactorSign;
    matrix<Type, Row - 1, Column - 1> c = cofactor<Type, Row, Column>(m, row, column, &cofactorSign);
    return determinant<Type>(c) * cofactorSign;
}

template <class Type, int Row, int Column>
matrix<Type, Row, Column> cofactor_matrix(const matrix<Type, Row, Column>& m)
{
    matrix<Type, Row, Column> cofactorMatrix;
    for (int column = 0; column < Column; column++)
        for (int row = 0; row < Row; row++)
        {
            float cofactorDeterminant = cofactor_determinant<Type, Row, Column>(m, row, column);
            cofactorMatrix[row][column] = cofactorDeterminant;
        }
    return cofactorMatrix;
}

template <class Type, int Row, int Column>
matrix<Type, Row, Column> adjoint(const matrix<Type, Row, Column>& m)
{
    return transpose<Type, Row, Column>(cofactor_matrix<Type, Row, Column>(m));
}

template <class Type>
matrix<Type, 4, 4> inverse(const matrix<Type, 4, 4>& m)
{
    Type determinant00 = cofactor_determinant(m, 0, 0);
    Type determinant10 = cofactor_determinant(m, 1, 0);
    Type determinant20 = cofactor_determinant(m, 2, 0);
    Type determinant30 = cofactor_determinant(m, 3, 0);

    Type determinant01 = cofactor_determinant(m, 0, 1);
    Type determinant11 = cofactor_determinant(m, 1, 1);
    Type determinant21 = cofactor_determinant(m, 2, 1);
    Type determinant31 = cofactor_determinant(m, 3, 1);

    Type determinant02 = cofactor_determinant(m, 0, 2);
    Type determinant12 = cofactor_determinant(m, 1, 2);
    Type determinant22 = cofactor_determinant(m, 2, 2);
    Type determinant32 = cofactor_determinant(m, 3, 2);

    Type determinant03 = cofactor_determinant(m, 0, 3);
    Type determinant13 = cofactor_determinant(m, 1, 3);
    Type determinant23 = cofactor_determinant(m, 2, 3);
    Type determinant33 = cofactor_determinant(m, 3, 3);

    matrix<Type, 4, 4> adjoint = {
        determinant00, determinant10, determinant20, determinant30,
        determinant01, determinant11, determinant21, determinant31,
        determinant02, determinant12, determinant22, determinant32,
        determinant03, determinant13, determinant23, determinant33,
    };

    Type determinant =
        m._m00 * determinant00 +
        m._m01 * determinant01 +
        m._m02 * determinant02 +
        m._m03 * determinant03;

    return adjoint / determinant;
}
