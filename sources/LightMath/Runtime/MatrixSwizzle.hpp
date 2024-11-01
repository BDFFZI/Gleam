#pragma once
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

#define MakeMatrixElement(type,a) MatrixElement<type,a##i> (a);

template <class Type>
struct Matrix3x3Row
{
    union
    {
#define xi 0
#define yi 3
#define zi 6
#define ri 0
#define gi 3
#define bi 6
        MakeVectorSwizzleGroup3(Type, x, y, z)
        MakeVectorSwizzleGroup3(Type, r, g, b)
        MakeMatrixElement(Type, x)
        MakeMatrixElement(Type, y)
        MakeMatrixElement(Type, z)
        MakeMatrixElement(Type, r)
        MakeMatrixElement(Type, g)
        MakeMatrixElement(Type, b)
#undef xi
#undef yi
#undef zi
#undef ri
#undef gi
#undef bi
    };

    Matrix3x3Row& operator=(vector<Type, 3> vector)
    {
        operator[](0) = vector.data[0];
        operator[](1) = vector.data[1];
        operator[](2) = vector.data[2];
        return *this;
    }
    operator vector<Type, 3>()
    {
        return {operator[](0), operator[](1), operator[](2)};
    }
    
    Type& operator[](const int i)
    {
        return reinterpret_cast<Type*>(this)[static_cast<uint32_t>(i * 3)];
    }
};

template <class Type>
struct Matrix4x4Row
{
    union
    {
#define xi 0
#define yi 4
#define zi 8
#define wi 12
#define ri 0
#define gi 4
#define bi 8
#define ai 12
        MakeVectorSwizzleGroup4(Type, x, y, z, w)
        MakeVectorSwizzleGroup4(Type, r, g, b, a)
        MakeMatrixElement(Type, x)
        MakeMatrixElement(Type, y)
        MakeMatrixElement(Type, z)
        MakeMatrixElement(Type, w)
        MakeMatrixElement(Type, r)
        MakeMatrixElement(Type, g)
        MakeMatrixElement(Type, b)
        MakeMatrixElement(Type, a)
#undef xi
#undef yi
#undef zi
#undef wi
#undef ri
#undef gi
#undef bi
#undef ai
    };

    Matrix4x4Row& operator=(vector<Type, 4> vector)
    {
        operator[](0) = vector.data[0];
        operator[](1) = vector.data[1];
        operator[](2) = vector.data[2];
        operator[](3) = vector.data[3];
        return *this;
    }
    operator vector<Type, 4>()
    {
        return {operator[](0), operator[](1), operator[](2), operator[](3)};
    }

    Type& operator[](const int i)
    {
        return reinterpret_cast<Type*>(this)[static_cast<uint32_t>(i * 4)];
    }
};
