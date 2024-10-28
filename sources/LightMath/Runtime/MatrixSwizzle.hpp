#pragma once
#include <stdexcept>
#include "VectorSwizzle.hpp"

template <class Type, int Index>
struct MatrixElement
{
    MatrixElement& operator=(const Type value)
    {
        reinterpret_cast<Type*>(this)[Index] = value;
        return *this;
    }
    operator const Type() const
    {
        return reinterpret_cast<const Type*>(this)[Index];
    }
};

#define MakeMatrixElement(map,type,a) MatrixElement<type,map(#a)> (a);

template <class Type>
struct Matrix3x3Row
{
    consteval static int Matrix3x3ComponentMap(const std::string_view& str)
    {
        if (str == "x" || str == "r") return 0;
        if (str == "y" || str == "g") return 3;
        if (str == "z" || str == "b") return 6;

        throw std::out_of_range("不支持的变量名！");
    }

    union
    {
        MakeVectorSwizzleGroup3(Matrix3x3ComponentMap, Type, x, y, z)
        MakeVectorSwizzleGroup3(Matrix3x3ComponentMap, Type, r, g, b)
        MakeMatrixElement(Matrix3x3ComponentMap, Type, x)
        MakeMatrixElement(Matrix3x3ComponentMap, Type, y)
        MakeMatrixElement(Matrix3x3ComponentMap, Type, z)
        MakeMatrixElement(Matrix3x3ComponentMap, Type, r)
        MakeMatrixElement(Matrix3x3ComponentMap, Type, g)
        MakeMatrixElement(Matrix3x3ComponentMap, Type, b)
    };


    Type& operator[](const int i)
    {
        return reinterpret_cast<Type*>(this)[static_cast<uint32_t>(i * 3)];
    }
};

template <class Type>
struct Matrix4x4Row
{
    consteval static int Matrix4x4ComponentMap(const std::string_view& str)
    {
        if (str == "x" || str == "r") return 0;
        if (str == "y" || str == "g") return 4;
        if (str == "z" || str == "b") return 8;
        if (str == "w" || str == "a") return 12;

        throw std::out_of_range("不支持的变量名！");
    }

    union
    {
        MakeVectorSwizzleGroup4(Matrix4x4ComponentMap, Type, x, y, z, w)
        MakeVectorSwizzleGroup4(Matrix4x4ComponentMap, Type, r, g, b, a)
        MakeMatrixElement(Matrix4x4ComponentMap, Type, x)
        MakeMatrixElement(Matrix4x4ComponentMap, Type, y)
        MakeMatrixElement(Matrix4x4ComponentMap, Type, z)
        MakeMatrixElement(Matrix4x4ComponentMap, Type, w)
        MakeMatrixElement(Matrix4x4ComponentMap, Type, r)
        MakeMatrixElement(Matrix4x4ComponentMap, Type, g)
        MakeMatrixElement(Matrix4x4ComponentMap, Type, b)
        MakeMatrixElement(Matrix4x4ComponentMap, Type, a)
    };

    Type& operator[](const int i)
    {
        return reinterpret_cast<Type*>(this)[static_cast<uint32_t>(i * 4)];
    }
};