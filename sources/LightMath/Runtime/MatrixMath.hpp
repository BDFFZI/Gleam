#pragma once

#include "VectorMath.hpp"
#include "Matrix.hpp"

template <class Type, int Row, int Column>
constexpr matrix<Type, Row, Column> transpose(const matrix<Type, Row, Column>& m)
{
    matrix<Type, Row, Column> result;
    for (int row = 0; row < Row; row++)
        for (int column = 0; column < Column; column++)
            result[row][column] = m[column][row];
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
