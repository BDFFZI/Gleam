#pragma once

#include "VectorMath.hpp"
#include "Matrix.hpp"

template <class Type, int Row, int Column>
constexpr matrix<Type, Row, Column> transpose(const matrix<Type, Row, Column>& m)
{
    matrix<Type, Row, Column> result;
    for (int row = 0; row < Row; row++)
        for (int column = 0; column < Column; column++)
            result[row][column] = m.data[column + row * Row];
    return result;
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
                cofactor.data[i] = m.data[rowIndex + columnIndex * Row];
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
            value *= m.data[j + permutation[j] * 4];
        total += value;
    }
    return total;
}

template <class Type>
matrix<Type, 4, 4> inverse(const matrix<Type, 4, 4>& m)
{
    Type determinant00 = cofactor_determinant<Type, 4, 4>(m, 0, 0);
    Type determinant10 = cofactor_determinant<Type, 4, 4>(m, 1, 0);
    Type determinant20 = cofactor_determinant<Type, 4, 4>(m, 2, 0);
    Type determinant30 = cofactor_determinant<Type, 4, 4>(m, 3, 0);

    Type determinant01 = cofactor_determinant<Type, 4, 4>(m, 0, 1);
    Type determinant11 = cofactor_determinant<Type, 4, 4>(m, 1, 1);
    Type determinant21 = cofactor_determinant<Type, 4, 4>(m, 2, 1);
    Type determinant31 = cofactor_determinant<Type, 4, 4>(m, 3, 1);

    Type determinant02 = cofactor_determinant<Type, 4, 4>(m, 0, 2);
    Type determinant12 = cofactor_determinant<Type, 4, 4>(m, 1, 2);
    Type determinant22 = cofactor_determinant<Type, 4, 4>(m, 2, 2);
    Type determinant32 = cofactor_determinant<Type, 4, 4>(m, 3, 2);

    Type determinant03 = cofactor_determinant<Type, 4, 4>(m, 0, 3);
    Type determinant13 = cofactor_determinant<Type, 4, 4>(m, 1, 3);
    Type determinant23 = cofactor_determinant<Type, 4, 4>(m, 2, 3);
    Type determinant33 = cofactor_determinant<Type, 4, 4>(m, 3, 3);

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

template <class Type>
matrix<Type, 3, 3> operator*(const matrix<Type, 3, 3>& left, const matrix<Type, 3, 3>& right)
{
    matrix<Type, 3, 3> matrix;

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
template <class Type>
matrix<Type, 4, 4> operator*(const matrix<Type, 4, 4>& left, const matrix<Type, 4, 4>& right)
{
    matrix<Type, 4, 4> matrix;

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

template <class Type>
vector<Type, 4> operator*(const matrix<Type, 4, 4>& left, const vector<Type, 4>& right)
{
    return {
        left.m00 * right.x + left.m01 * right.y + left.m02 * right.z + left.m03 * right.w,
        left.m10 * right.x + left.m11 * right.y + left.m12 * right.z + left.m13 * right.w,
        left.m20 * right.x + left.m21 * right.y + left.m22 * right.z + left.m23 * right.w,
        left.m30 * right.x + left.m31 * right.y + left.m32 * right.z + left.m33 * right.w,
    };
}

template <class Type>
constexpr std::string to_string(const matrix<Type, 3, 3>& m)
{
    return std::format(
        "|{:f} {:f} {:f}|\n|{:f} {:f} {:f}|\n|{:f} {:f} {:f}|",
        m._m00, m._m01, m._m02,
        m._m10, m._m11, m._m12,
        m._m20, m._m21, m._m22
    );
}
template <class Type>
constexpr std::string to_string(const matrix<Type, 4, 4>& m)
{
    return std::format(
        "|{:f} {:f} {:f} {:f}|\n|{:f} {:f} {:f} {:f}|\n|{:f} {:f} {:f} {:f}|\n|{:f} {:f} {:f} {:f}|",
        m._m00, m._m01, m._m02, m._m03,
        m._m10, m._m11, m._m12, m._m13,
        m._m20, m._m21, m._m22, m._m23,
        m._m30, m._m31, m._m32, m._m33
    );
}

MakeVectorFunctions(float, 3*3)
MakeVectorFunctions(float, 4*4)
using float3x3 = vector<float, 3 * 3>;
using float4x4 = vector<float, 4 * 4>;

MakeVectorFunction_All(bool, 3*3)
MakeVectorFunction_All(bool, 4*4)
using bool3x3 = vector<bool, 3 * 3>;
using bool4x4 = vector<bool, 4 * 4>;
