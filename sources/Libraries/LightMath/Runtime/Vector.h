#pragma once

#include "VectorSwizzle.h"

namespace Light
{
    using int2 = vector<int, 2>;
    using int3 = vector<int, 3>;
    using int4 = vector<int, 4>;
    using float2 = vector<float, 2>;
    using float3 = vector<float, 3>;
    using float4 = vector<float, 4>;

    //由于头文件依赖的原因，vector模板原型放在了VectorSwizzle.h文件中

#define xi 0
#define yi 1
#define zi 2
#define wi 3
#define ri 0
#define gi 1
#define bi 2
#define ai 3

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


            Light_MakeVectorSwizzleGroup2(Type, x, y)
            Light_MakeVectorSwizzleGroup2(Type, r, g)
        };

        constexpr vector(const Type x, const Type y)
        {
            this->x = x;
            this->y = y;
        }

        template <class OtherType>
        constexpr explicit operator vector<OtherType, 2>()
        {
            return {
                static_cast<OtherType>(x),
                static_cast<OtherType>(y),
            };
        }

        Light_MakeVectorMemberFunctions(Type, 2)
        Light_MakeVectorMemberFunctions_Indexer(Type)
    };

    template <class Type>
    struct vector<Type, 3>
    {
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

            Light_MakeVectorSwizzleGroup3(Type, x, y, z)
            Light_MakeVectorSwizzleGroup3(Type, r, g, b)
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

        template <class OtherType>
        constexpr explicit operator vector<OtherType, 3>()
        {
            return {
                static_cast<OtherType>(x),
                static_cast<OtherType>(y),
                static_cast<OtherType>(z),
            };
        }

        Light_MakeVectorMemberFunctions(Type, 3)
        Light_MakeVectorMemberFunctions_Indexer(Type)
    };

    template <class Type>
    struct vector<Type, 4>
    {
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

            Light_MakeVectorSwizzleGroup4(vector, x, y, z, w)
            Light_MakeVectorSwizzleGroup4(vector, r, g, b, a)
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

        template <class OtherType>
        constexpr explicit operator vector<OtherType, 4>()
        {
            return {
                static_cast<OtherType>(x),
                static_cast<OtherType>(y),
                static_cast<OtherType>(z),
                static_cast<OtherType>(w),
            };
        }

        Light_MakeVectorMemberFunctions(Type, 4)
        Light_MakeVectorMemberFunctions_Indexer(Type)
    };

#undef xi
#undef yi
#undef zi
#undef wi
#undef ri
#undef gi
#undef bi
#undef ai

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
}

#include <type_traits>
template <class Type, int Number>
struct std::hash<Light::vector<Type, Number>> // NOLINT(cert-dcl58-cpp)
{
    size_t operator()(const Light::vector<Type, Number>& value) const noexcept
    {
        size_t seed = 0;
        hash<float> hasher;
        for (int i = 0; i < Number; i++)
            Light::CombineVectorHash(&seed, hasher(value[i]));
        return seed;
    }
};
