#pragma once

#include "VectorMath.h"
#include "Matrix.h"

namespace Light
{
#define MakeMatrixFunction_Transpose(Type,Row,Column)\
constexpr matrix<Type, Row, Column> transpose(const matrix<Type, Row, Column>& m)\
{\
    matrix<Type, Row, Column> result;\
    for (int row = 0; row < (Row); row++)\
        for (int column = 0; column < (Column); column++)\
            result[row][column] = m.data[column + row * (Row)];\
    return result;\
}
#define MakeMatrixFunction_Cofactor(Type,Row,Column)\
constexpr matrix<Type, (Row) - 1, (Column) - 1> cofactor(const matrix<Type, Row, Column>& m, const int row, const int column, int* sign)\
{\
    if (sign != nullptr)\
        *sign = (row + column) % 2 == 0 ? 1 : -1;\
    matrix<Type, (Row) - 1, (Column) - 1> cofactor;\
    int i = 0;\
    for (int columnIndex = 0; columnIndex < (Column); columnIndex++)\
        for (int rowIndex = 0; rowIndex < (Row); rowIndex++)\
            if (columnIndex != column && rowIndex != row)\
            {\
                cofactor.data[i] = m.data[rowIndex + columnIndex * (Row)];\
                i++;\
            }\
    return cofactor;\
}
#define MakeMatrixFunction_CofactorDeterminant(Type,Row,Column)\
constexpr Type cofactor_determinant(const matrix<Type, Row, Column>& m, const int row, const int column)\
{\
    int cofactorSign;\
    matrix<Type, (Row) - 1, (Column) - 1> c = cofactor(m, row, column, &cofactorSign);\
    return determinant(c) * static_cast<float>(cofactorSign);\
}
#define MakeMatrixFunction_CofactorMatrix(Type,Row,Column)\
constexpr matrix<Type, Row, Column> cofactor_matrix(const matrix<Type, Row, Column>& m)\
{\
    matrix<Type, Row, Column> cofactorMatrix;\
    for (int column = 0; column < (Column); column++)\
        for (int row = 0; row < (Row); row++)\
        {\
            float cofactorDeterminant = cofactor_determinant(m, row, column);\
            cofactorMatrix[row][column] = cofactorDeterminant;\
        }\
    return cofactorMatrix;\
}
#define MakeMatrixFunction_Adjoint(Type,Row,Column)\
constexpr matrix<Type, Row, Column> adjoint(const matrix<Type, Row, Column>& m)\
{\
    return transpose(cofactor_matrix(m));\
}

#define MakeMatrixFunctions(Type,Row,Column)\
Light_MakeVectorFunctions_Decimals(Type, (Row) * (Column))\
MakeMatrixFunction_Transpose(Type,Row,Column)\
MakeMatrixFunction_Cofactor(Type,Row,Column)\
MakeMatrixFunction_CofactorDeterminant(Type,Row,Column)\
MakeMatrixFunction_CofactorMatrix(Type,Row,Column)\
MakeMatrixFunction_Adjoint(Type,Row,Column)


    template <class Type>
    constexpr Type determinant(const matrix<Type, 2, 2>& m)
    {
        return m[0] * m[3] - m[2] * m[1];
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
    constexpr matrix<Type, 4, 4> inverse(const matrix<Type, 4, 4>& m)
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

        matrix<Type, 4, 4> inverse = adjoint / determinant;
        return inverse;
    }

    template <class Type>
    constexpr matrix<Type, 3, 3> mul(const matrix<Type, 3, 3>& left, const matrix<Type, 3, 3>& right)
    {
        matrix<Type, 3, 3> matrix;

        matrix._m00 = left._m00 * right._m00 + left._m01 * right._m10 + left._m02 * right._m20;
        matrix._m01 = left._m00 * right._m01 + left._m01 * right._m11 + left._m02 * right._m21;
        matrix._m02 = left._m00 * right._m02 + left._m01 * right._m12 + left._m02 * right._m22;

        matrix._m10 = left._m10 * right._m00 + left._m11 * right._m10 + left._m12 * right._m20;
        matrix._m11 = left._m10 * right._m01 + left._m11 * right._m11 + left._m12 * right._m21;
        matrix._m12 = left._m10 * right._m02 + left._m11 * right._m12 + left._m12 * right._m22;

        matrix._m20 = left._m20 * right._m00 + left._m21 * right._m10 + left._m22 * right._m20;
        matrix._m21 = left._m20 * right._m01 + left._m21 * right._m11 + left._m22 * right._m21;
        matrix._m22 = left._m20 * right._m02 + left._m21 * right._m12 + left._m22 * right._m22;

        return matrix;
    }
    template <class Type>
    constexpr vector<Type, 3> mul(const matrix<Type, 3, 3>& left, const vector<Type, 3>& right)
    {
        return {
            left._m00 * right.x + left._m01 * right.y + left._m02 * right.z,
            left._m10 * right.x + left._m11 * right.y + left._m12 * right.z,
            left._m20 * right.x + left._m21 * right.y + left._m22 * right.z,
        };
    }
    template <class Type>
    matrix<Type, 4, 4> mul(const matrix<Type, 4, 4>& left, const matrix<Type, 4, 4>& right)
    {
        matrix<Type, 4, 4> matrix;

        matrix._m00 = left._m00 * right._m00 + left._m01 * right._m10 + left._m02 * right._m20 + left._m03 * right._m30;
        matrix._m01 = left._m00 * right._m01 + left._m01 * right._m11 + left._m02 * right._m21 + left._m03 * right._m31;
        matrix._m02 = left._m00 * right._m02 + left._m01 * right._m12 + left._m02 * right._m22 + left._m03 * right._m32;
        matrix._m03 = left._m00 * right._m03 + left._m01 * right._m13 + left._m02 * right._m23 + left._m03 * right._m33;

        matrix._m10 = left._m10 * right._m00 + left._m11 * right._m10 + left._m12 * right._m20 + left._m13 * right._m30;
        matrix._m11 = left._m10 * right._m01 + left._m11 * right._m11 + left._m12 * right._m21 + left._m13 * right._m31;
        matrix._m12 = left._m10 * right._m02 + left._m11 * right._m12 + left._m12 * right._m22 + left._m13 * right._m32;
        matrix._m13 = left._m10 * right._m03 + left._m11 * right._m13 + left._m12 * right._m23 + left._m13 * right._m33;

        matrix._m20 = left._m20 * right._m00 + left._m21 * right._m10 + left._m22 * right._m20 + left._m23 * right._m30;
        matrix._m21 = left._m20 * right._m01 + left._m21 * right._m11 + left._m22 * right._m21 + left._m23 * right._m31;
        matrix._m22 = left._m20 * right._m02 + left._m21 * right._m12 + left._m22 * right._m22 + left._m23 * right._m32;
        matrix._m23 = left._m20 * right._m03 + left._m21 * right._m13 + left._m22 * right._m23 + left._m23 * right._m33;

        matrix._m30 = left._m30 * right._m00 + left._m31 * right._m10 + left._m32 * right._m20 + left._m33 * right._m30;
        matrix._m31 = left._m30 * right._m01 + left._m31 * right._m11 + left._m32 * right._m21 + left._m33 * right._m31;
        matrix._m32 = left._m30 * right._m02 + left._m31 * right._m12 + left._m32 * right._m22 + left._m33 * right._m32;
        matrix._m33 = left._m30 * right._m03 + left._m31 * right._m13 + left._m32 * right._m23 + left._m33 * right._m33;

        return matrix;
    }
    template <class Type>
    constexpr vector<Type, 4> mul(const matrix<Type, 4, 4>& left, const vector<Type, 4>& right)
    {
        return {
            left._m00 * right.x + left._m01 * right.y + left._m02 * right.z + left._m03 * right.w,
            left._m10 * right.x + left._m11 * right.y + left._m12 * right.z + left._m13 * right.w,
            left._m20 * right.x + left._m21 * right.y + left._m22 * right.z + left._m23 * right.w,
            left._m30 * right.x + left._m31 * right.y + left._m32 * right.z + left._m33 * right.w,
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

    MakeMatrixFunctions(float, 3, 3)
    MakeMatrixFunctions(float, 4, 4)
    Light_MakeVectorFunction_All(bool, 3*3)
    Light_MakeVectorFunction_All(bool, 4*4)
    Light_MakeVectorFunction_Any(bool, 3*3)
    Light_MakeVectorFunction_Any(bool, 4*4)

    constexpr static float3 DecomposeRotation(const float3x3& rotation)
    {
        float z;
        float y;
        float x;

        float sinX = -rotation._m12;
        if (equal(::abs(rotation._m12), 1.0f) == false)
        {
            z = atan2(rotation._m10, rotation._m11);
            y = atan2(rotation._m02, rotation._m22);
            float cosX = !equal(rotation._m10, 0.0f)
                             ? rotation._m10 / sin(z)
                             : rotation._m11 / cos(z);
            x = atan2(sinX, cosX);
        }
        else
        {
            z = 0;
            if (sinX > 0)
            {
                y = atan2(rotation._m01, rotation._m00);
                x = std::numbers::pi_v<float> / 2;
            }
            else
            {
                y = atan2(-rotation._m01, rotation._m00);
                x = -std::numbers::pi_v<float> / 2;
            }
        }

        return degrees(vector<float, 3>{x, y, z});
    }
    constexpr static void DecomposeTRS(const float4x4& trs, float3& position, float3x3& rotation, float3& scale)
    {
        position = trs._m03_m13_m23;
        float3x3 rs = static_cast<float3x3>(trs);
        float3x3 scale2 = mul(transpose(rs), rs);
        scale = sqrt(float3(scale2._m00, scale2._m11, scale2._m22));
        rotation = mul(rs, float3x3::Scale(1 / scale));
    }
    constexpr static void DecomposeTRS(const float4x4& trs, float3& position, float3& eulerAngles, float3& scale)
    {
        float3x3 rotation;
        DecomposeTRS(trs, position, rotation, scale);
        eulerAngles = DecomposeRotation(rotation);
    }
}