#pragma once
#include "Vector.hpp"

template <class Type, int Row, int Column>
using matrix = vector<Type, Row * Column>;

template <class Type>
struct vector<Type, 3 * 3>
{
    //为了与图像接口兼容故采用按列存储
    union
    {
        Type data[3 * 3];
        struct
        {
            Type _m00, _m10, _m20;
            Type _m01, _m11, _m21;
            Type _m02, _m12, _m22;
        };
        struct
        {
            Type _11, _21, _31;
            Type _12, _22, _32;
            Type _13, _23, _33;
        };
    };

    constexpr vector(
        const float m00, const float m01, const float m02,
        const float m10, const float m11, const float m12,
        const float m20, const float m21, const float m22)
        : _m00(m00), _m10(m10), _m20(m20),
          _m01(m01), _m11(m11), _m21(m21),
          _m02(m02), _m12(m12), _m22(m22)
    {
    }

    MakeVectorMemberFunctions(Type, 3*3)
};

template <class Type>
struct vector<Type, 4 * 4>
{
    // constexpr static vector zero = {
    //     0, 0, 0, 0,
    //     0, 0, 0, 0,
    //     0, 0, 0, 0,
    //     0, 0, 0, 0
    // };
    // constexpr static vector identity = {
    //     1, 0, 0, 0,
    //     0, 1, 0, 0,
    //     0, 0, 1, 0,
    //     0, 0, 0, 1
    // };

    //为了与图像接口兼容故采用按列存储
    union
    {
        Type data[4 * 4];
        struct
        {
            Type _m00, _m10, _m20, _m30;
            Type _m01, _m11, _m21, _m31;
            Type _m02, _m12, _m22, _m32;
            Type _m03, _m13, _m23, _m33;
        };
        struct
        {
            Type _11, _21, _31, _41;
            Type _12, _22, _32, _42;
            Type _13, _23, _33, _43;
            Type _14, _24, _34, _44;
        };
    };
    
    constexpr vector(const float m00, const float m01, const float m02, const float m03,
                     const float m10, const float m11, const float m12, const float m13,
                     const float m20, const float m21, const float m22, const float m23,
                     const float m30, const float m31, const float m32, const float m33)
        : _m00(m00), _m10(m10), _m20(m20), _m30(m30),
          _m01(m01), _m11(m11), _m21(m21), _m31(m31),
          _m02(m02), _m12(m12), _m22(m22), _m32(m32),
          _m03(m03), _m13(m13), _m23(m23), _m33(m33)
    {
    }
    constexpr vector(const vector<Type, 3 * 3>& matrix)
        : _m00(matrix._m00), _m10(matrix._m10), _m20(matrix._m20), _m30(0),
          _m01(matrix._m01), _m11(matrix._m11), _m21(matrix._m21), _m31(0),
          _m02(matrix._m02), _m12(matrix._m12), _m22(matrix._m22), _m32(0),
          _m03(0), _m13(0), _m23(0), _m33(1)
    {
    }

    MakeVectorMemberFunctions(Type, 3*3)

    vector<Type, 4> operator[](int row)
    {
        return {
            (&_m00)[row],
            (&_m01)[row],
            (&_m02)[row],
            (&_m03)[row],
        };
    }
};

constexpr std::string to_string(const float3x3& matrix)
{
    return std::format(
        "|{:f} {:f} {:f}|\n|{:f} {:f} {:f}|\n|{:f} {:f} {:f}|",
        matrix._m00, matrix._m01, matrix._m02,
        matrix._m10, matrix._m11, matrix._m12,
        matrix._m20, matrix._m21, matrix._m22
    );
}
constexpr std::string to_string(const float4x4& matrix)
{
    return std::format(
        "|{:f} {:f} {:f} {:f}|\n|{:f} {:f} {:f} {:f}|\n|{:f} {:f} {:f} {:f}|\n|{:f} {:f} {:f} {:f}|",
        matrix._m00, matrix._m01, matrix._m02, matrix._m03,
        matrix._m10, matrix._m11, matrix._m12, matrix._m13,
        matrix._m20, matrix._m21, matrix._m22, matrix._m23,
        matrix._m30, matrix._m31, matrix._m32, matrix._m33
    );
}
