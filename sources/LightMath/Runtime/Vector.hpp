#pragma once
#include <format>
#include "VectorSwizzle.hpp"

//由于头文件依赖的原因，vector模板原型放在了VectorSwizzle.hpp文件中

template <class Type>
struct vector<Type, 2>
{
    union
    {
        Type data[2];
        struct
        {
            Type x;
            Type y;
        };
        struct
        {
            Type r;
            Type g;
        };
        PlaceSwizzleGroup2(VertexComponentMap, Type, x, y)
        PlaceSwizzleGroup2(VertexComponentMap, Type, r, g)
    };

    constexpr vector(const Type x, const Type y)
    {
        this->x = x;
        this->y = y;
    }

    VectorMemberFunction(Type, 2)
};

template <class Type>
struct vector<Type, 3>
{
    // constexpr static vector zero = {0, 0, 0};
    // constexpr static vector one = {1, 1, 1};
    // constexpr static vector left = {-1, 0, 0};
    // constexpr static vector right = {1, 0, 0};
    // constexpr static vector up = {0, 1, 0};
    // constexpr static vector down = {0, -1, 0};
    // constexpr static vector front = {0, 0, 1};
    // constexpr static vector back = {0, 0, -1};

    union
    {
        Type data[3];
        struct
        {
            Type x;
            Type y;
            Type z;
        };
        struct
        {
            Type r;
            Type g;
            Type b;
        };
        PlaceSwizzleGroup3(VertexComponentMap, Type, x, y, z)
        PlaceSwizzleGroup3(VertexComponentMap, Type, r, g, b)
    };

    constexpr vector(const Type x, const Type y, const Type z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    constexpr vector(const vector<Type, 2> xy, const Type z)
    {
        x = xy.x;
        y = xy.y;
        this->z = z;
    }

    VectorMemberFunction(Type, 3)
};

template <class Type>
struct vector<Type, 4>
{
    // constexpr static vector white = {1.0, 1.0, 1.0, 1.0};
    // constexpr static vector black = {0.0, 0.0, 0.0, 1.0};
    // constexpr static vector red = {1.0, 0.0, 0.0, 1.0};
    // constexpr static vector green = {0.0, 1.0, 0.0, 1.0};
    // constexpr static vector blue = {0.0, 0.0, 1.0, 1.0};

    union
    {
        Type data[4];
        struct
        {
            Type x;
            Type y;
            Type z;
            Type w;
        };
        struct
        {
            Type r;
            Type g;
            Type b;
            Type a;
        };
        PlaceSwizzleGroup4(VertexComponentMap, vector, x, y, z, w)
        PlaceSwizzleGroup4(VertexComponentMap, vector, r, g, b, a)
    };

    constexpr vector(Type x, Type y, Type z, Type w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    constexpr vector(vector<Type, 3> xyz, Type w)
    {
        x = xyz.x;
        y = xyz.y;
        z = xyz.z;
        this->w = w;
    }

    VectorMemberFunction(Type, 4)
};

/**
 * 将哈希值hash合并到seed中
 * @param seed 用于存储合并后的哈希值
 * @param hash 
 */
constexpr void CombineVectorHash(size_t* seed, size_t hash)
{
    //复制的glm的

    hash += 0x9e3779b9 + (*seed << 6) + (*seed >> 2);
    *seed ^= hash;
}

template <class Type, int Number>
struct std::hash<vector<Type, Number>>
{
    size_t operator()(const ::vector<Type, Number>& value) const noexcept
    {
        size_t seed = 0;
        hash<float> hasher;
        for (int i = 0; i < Number; i++)
            CombineVectorHash(&seed, hasher(value[i]));
        return seed;
    }
};
