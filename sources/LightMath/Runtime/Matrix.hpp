#pragma once
#include "MatrixSwizzle.hpp"
#include "VectorMath.hpp"

template <class Type, int Row, int Column>
using matrix = vector<Type, Row * Column>;
using float3x3 = matrix<float, 3, 3>;
using float4x4 = matrix<float, 4, 4>;

template <class Type>
struct vector<Type, 3 * 3>
{
    /**
    * 创建一个旋转矩阵
     * @param degree 基于欧拉角的三轴旋转角度
     * @return 
     */
    constexpr static vector Rotate(vector<Type, 3> degree)
    {
        vector<Type, 3> rad = radians(degree);

        float cosX = cos(rad.x);
        float sinX = sin(rad.x);
        vector xRotate = {
            1, 0, 0,
            0, cosX, -sinX,
            0, sinX, cosX,
        };

        float cosY = cos(rad.y);
        float sinY = sin(rad.y);
        vector yRotate = {
            cosY, 0, sinY,
            0, 1, 0,
            -sinY, 0, cosY,
        };

        float cosZ = cos(rad.z);
        float sinZ = sin(rad.z);
        vector zRotate = {
            cosZ, -sinZ, 0,
            sinZ, cosZ, 0,
            0, 0, 1,
        };

        return mul(yRotate, mul(xRotate, zRotate));
    }
    /**
     * 创建一个缩放矩阵
     * @param scale 三维空间的各轴缩放
     * @return 
     */
    constexpr static vector Scale(vector<Type, 3> scale)
    {
        return {
            scale.x, 0, 0,
            0, scale.y, 0,
            0, 0, scale.z,
        };
    }

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

#define _m00i 0
#define _m10i 1
#define _m20i 2
#define _m01i 3
#define _m11i 4
#define _m21i 5
#define _m02i 6
#define _m12i 7
#define _m22i 8
#define _11i 0
#define _21i 1
#define _31i 2
#define _12i 3
#define _22i 4
#define _32i 5
#define _13i 6
#define _23i 7
#define _33i 8

        MakeVectorSwizzleGroup3(Type, _m00, _m10, _m20)
        MakeVectorSwizzleGroup3(Type, _m01, _m11, _m21)
        MakeVectorSwizzleGroup3(Type, _m02, _m12, _m22)
        MakeVectorSwizzleGroup3(Type, _11, _21, _31)
        MakeVectorSwizzleGroup3(Type, _12, _22, _32)
        MakeVectorSwizzleGroup3(Type, _13, _23, _33)
        MakeVectorSwizzleGroup3(Type, _m00, _m01, _m02)
        MakeVectorSwizzleGroup3(Type, _m10, _m11, _m12)
        MakeVectorSwizzleGroup3(Type, _m20, _m21, _m22)
        MakeVectorSwizzleGroup3(Type, _11, _12, _13)
        MakeVectorSwizzleGroup3(Type, _21, _22, _23)
        MakeVectorSwizzleGroup3(Type, _31, _32, _33)
        MakeVectorSwizzleGroup3(Type, _m00, _m11, _m22)
        MakeVectorSwizzleGroup3(Type, _m10, _m21, _m02)
        MakeVectorSwizzleGroup3(Type, _m20, _m01, _m12)
        MakeVectorSwizzleGroup3(Type, _11, _22, _33)
        MakeVectorSwizzleGroup3(Type, _12, _23, _31)
        MakeVectorSwizzleGroup3(Type, _13, _21, _32)

#undef _m00i
#undef _m10i
#undef _m20i
#undef _m01i
#undef _m11i
#undef _m21i
#undef _m02i
#undef _m12i
#undef _m22i
#undef _11i
#undef _21i
#undef _31i
#undef _12i
#undef _22i
#undef _32i
#undef _13i
#undef _23i
#undef _33i
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

    Matrix3x3Row<Type>& operator[](const int i)
    {
        return *reinterpret_cast<Matrix3x3Row<Type>*>(&data[i]);
    }
};

template <class Type>
struct vector<Type, 4 * 4>
{
    constexpr static vector Identity()
    {
        return {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
    }
    constexpr static vector Translate(vector<Type, 3> position)
    {
        return {
            1, 0, 0, position.x,
            0, 1, 0, position.y,
            0, 0, 1, position.z,
            0, 0, 0, 1
        };
    }
    constexpr static vector Rotate(vector<Type, 3> degree)
    {
        return vector<Type, 3 * 3>::Rotate(degree);
    }
    constexpr static vector Scale(vector<Type, 3> scale)
    {
        return {
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0, 1,
        };
    }
    constexpr static vector TRS(vector<Type, 3> position, vector<Type, 3> rotation, vector<Type, 3> scale)
    {
        return mul(Translate(position), mul(Rotate(rotation), Scale(scale)));
    }
    /**
     * 创建一个正交投影矩阵，其剪辑空间遵从从右到左、从下到上都为[-1,1]，深度从近到远为[0-1]的约定。
     * @param right 视野范围半宽
     * @param top 视野范围半高
     * @param nearClipPlane 视野范围近平面
     * @param farClipPlane 视野范围远平面
     * @return 
     */
    constexpr static vector Ortho(const float right, const float top, const float nearClipPlane, const float farClipPlane)
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
     * @param fieldOfView 视锥体上下夹角
     * @param aspectRatio 视野范围宽高比（宽度/高度）
     * @param nearClipPlane 视野范围近平面
     * @param farClipPlane 视野范围远平面
     * @return 
     */
    constexpr static vector Perspective(const float fieldOfView, const float aspectRatio, const float nearClipPlane, const float farClipPlane)
    {
        float halfHeight = tan(radians(fieldOfView) * 0.5f) * nearClipPlane;
        float halfWidth = halfHeight * aspectRatio;

        return mul(Ortho(halfWidth, halfHeight, nearClipPlane, farClipPlane), vector{
                       nearClipPlane, 0, 0, 0,
                       0, nearClipPlane, 0, 0,
                       0, 0, nearClipPlane + farClipPlane, -nearClipPlane * farClipPlane,
                       0, 0, 1, 0
                   });
    }

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

#define _m00i 0
#define _m10i 1
#define _m20i 2
#define _m30i 3
#define _m01i 4
#define _m11i 5
#define _m21i 6
#define _m31i 7
#define _m02i 8
#define _m12i 9
#define _m22i 10
#define _m32i 11
#define _m03i 12
#define _m13i 13
#define _m23i 14
#define _m33i 15
#define _11i 0
#define _21i 1
#define _31i 2
#define _41i 3
#define _12i 4
#define _22i 5
#define _32i 6
#define _42i 7
#define _13i 8
#define _23i 9
#define _33i 10
#define _43i 11
#define _14i 12
#define _24i 13
#define _34i 14
#define _44i 15

        MakeVectorSwizzleGroup4(Type, _m00, _m10, _m20, _m30)
        MakeVectorSwizzleGroup4(Type, _m01, _m11, _m21, _m31)
        MakeVectorSwizzleGroup4(Type, _m02, _m12, _m22, _m32)
        MakeVectorSwizzleGroup4(Type, _m03, _m13, _m23, _m33)
        MakeVectorSwizzleGroup4(Type, _11, _21, _31, _41)
        MakeVectorSwizzleGroup4(Type, _12, _22, _32, _42)
        MakeVectorSwizzleGroup4(Type, _13, _23, _33, _43)
        MakeVectorSwizzleGroup4(Type, _14, _24, _34, _44)
        MakeVectorSwizzleGroup4(Type, _m00, _m01, _m02, _m03)
        MakeVectorSwizzleGroup4(Type, _m10, _m11, _m12, _m13)
        MakeVectorSwizzleGroup4(Type, _m20, _m21, _m22, _m23)
        MakeVectorSwizzleGroup4(Type, _m30, _m31, _m32, _m33)
        MakeVectorSwizzleGroup4(Type, _11, _12, _13, _14)
        MakeVectorSwizzleGroup4(Type, _21, _22, _23, _24)
        MakeVectorSwizzleGroup4(Type, _31, _32, _33, _34)
        MakeVectorSwizzleGroup4(Type, _41, _42, _43, _44)
        MakeVectorSwizzleGroup4(Type, _m00, _m11, _m22, _m33)
        MakeVectorSwizzleGroup4(Type, _m01, _m12, _m23, _m30)
        MakeVectorSwizzleGroup4(Type, _m02, _m13, _m20, _m31)
        MakeVectorSwizzleGroup4(Type, _m03, _m10, _m21, _m32)
        MakeVectorSwizzleGroup4(Type, _11, _22, _33, _44)
        MakeVectorSwizzleGroup4(Type, _12, _23, _34, _41)
        MakeVectorSwizzleGroup4(Type, _13, _24, _31, _42)
        MakeVectorSwizzleGroup4(Type, _14, _21, _32, _43)

#undef _m00i
#undef _m10i
#undef _m20i
#undef _m30i
#undef _m01i
#undef _m11i
#undef _m21i
#undef _m31i
#undef _m02i
#undef _m12i
#undef _m22i
#undef _m32i
#undef _m03i
#undef _m13i
#undef _m23i
#undef _m33i
#undef _11i
#undef _21i
#undef _31i
#undef _41i
#undef _12i
#undef _22i
#undef _32i
#undef _42i
#undef _13i
#undef _23i
#undef _33i
#undef _43i
#undef _14i
#undef _24i
#undef _34i
#undef _44i
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

    MakeVectorMemberFunctions(Type, 4*4)

    Matrix4x4Row<Type>& operator[](const int i)
    {
        return *reinterpret_cast<Matrix4x4Row<Type>*>(&data[i]);
    }
};
