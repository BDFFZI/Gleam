#pragma once

namespace Light
{
#define Light_MakeVectorMemberFunctions(Type,Number) \
constexpr vector() { };\
constexpr vector(const Type value)\
{\
for (int i = 0; i < (Number); i++)\
data[i] = value;\
}\

#define Light_MakeVectorMemberFunctions_Indexer(Type)\
constexpr Type##& operator[](const int i)\
{\
return data[i];\
}\
constexpr const Type##& operator[](const int i) const\
{\
return data[i];\
}

    template <class Type, int Number>
    struct vector
    {
        Type data[Number];

        Light_MakeVectorMemberFunctions(Type, Number)
        Light_MakeVectorMemberFunctions_Indexer(Type)
    };

    template <class Type, int... Index>
    struct VectorSwizzle
    {
        constexpr static int indices[] = {Index...};

        Type& At(const int i)
        {
            return reinterpret_cast<Type*>(this)[i];
        }
        const Type& At(const int i) const
        {
            return reinterpret_cast<const Type*>(this)[i];
        }

        constexpr VectorSwizzle& operator=(const vector<Type, sizeof...(Index)>& value)
        {
            for (int i = 0; i < sizeof...(Index); i++)
                At(indices[i]) = value[i];
            return *this;
        }
        constexpr operator vector<Type, sizeof...(Index)>() const
        {
            vector<Type, sizeof...(Index)> result;
            for (int i = 0; i < sizeof...(Index); i++)
                result[i] = At(indices[i]);
            return result;
        }

        constexpr VectorSwizzle& operator+=(const vector<Type, sizeof...(Index)>& value)
        {
            for (int i = 0; i < sizeof...(Index); i++)
                At(indices[i]) += value[i];
            return *this;
        }
        constexpr VectorSwizzle& operator-=(const vector<Type, sizeof...(Index)>& value)
        {
            for (int i = 0; i < sizeof...(Index); i++)
                At(indices[i]) -= value[i];
            return *this;
        }
        constexpr VectorSwizzle& operator*=(const vector<Type, sizeof...(Index)>& value)
        {
            for (int i = 0; i < sizeof...(Index); i++)
                At(indices[i]) *= value[i];
            return *this;
        }
        constexpr VectorSwizzle& operator/=(const vector<Type, sizeof...(Index)>& value)
        {
            for (int i = 0; i < sizeof...(Index); i++)
                At(indices[i]) /= value[i];
            return *this;
        }
        constexpr VectorSwizzle& operator++()
        {
            for (int i = 0; i < sizeof...(Index); i++)
                ++At(indices[i]);
            return *this;
        }
        constexpr VectorSwizzle& operator--()
        {
            for (int i = 0; i < sizeof...(Index); i++)
                --At(indices[i]);
            return *this;
        }
        constexpr vector<Type, sizeof...(Index)> operator++(int)
        {
            vector<Type, sizeof...(Index)> temp = *this;
            for (int i = 0; i < sizeof...(Index); i++)
                ++At(indices[i]);
            return temp;
        }
        constexpr vector<Type, sizeof...(Index)> operator--(int)
        {
            vector<Type, sizeof...(Index)> temp = *this;
            for (int i = 0; i < sizeof...(Index); i++)
                --At(indices[i]);
            return temp;
        }
        constexpr vector<Type, sizeof...(Index)> operator-()
        {
            vector<Type, sizeof...(Index)> result = *this;
            for (int i = 0; i < sizeof...(Index); i++)
                result[i] = -result[i];
            return result;
        }
    };

    ///快速定义swizzle的宏，可以自动生成排列组合出的变量。
    ///使用前需提供类似“xi”的宏定义来提供内存偏移信息。之前也有试过用常量函数，但编译效率太低了。

#define Light_MakeVectorSwizzle2(type,a,b) VectorSwizzle<type,a##i,b##i> a##b;
#define Light_MakeVectorSwizzle3(type,a,b,c) VectorSwizzle<type,a##i,b##i,c##i> a##b##c;
#define Light_MakeVectorSwizzle4(type,a,b,c,d) VectorSwizzle<type,a##i,b##i,c##i,d##i> a##b##c##d;

#define Light_MakeVectorSwizzle2Group(type,a,b) \
Light_MakeVectorSwizzle2(type,a,b)\
Light_MakeVectorSwizzle2(type,b,a)

#define Light_MakeVectorSwizzle3Group(type,a,b,c) \
Light_MakeVectorSwizzle3(type,a,b,c)\
Light_MakeVectorSwizzle3(type,a,c,b)\
Light_MakeVectorSwizzle3(type,b,a,c)\
Light_MakeVectorSwizzle3(type,b,c,a)\
Light_MakeVectorSwizzle3(type,c,a,b)\
Light_MakeVectorSwizzle3(type,c,b,a)

#define Light_MakeVectorSwizzle4QuarterGroup(type,head,a,b,c)\
    Light_MakeVectorSwizzle4(type,head,a,b,c)\
    Light_MakeVectorSwizzle4(type,head,a,c,b)\
    Light_MakeVectorSwizzle4(type,head,b,a,c)\
    Light_MakeVectorSwizzle4(type,head,b,c,a)\
    Light_MakeVectorSwizzle4(type,head,c,a,b)\
    Light_MakeVectorSwizzle4(type,head,c,b,a)

#define Light_MakeVectorSwizzle4Group(type,a,b,c,d) \
Light_MakeVectorSwizzle4QuarterGroup(type,a,b,c,d)\
Light_MakeVectorSwizzle4QuarterGroup(type,b,a,c,d)\
Light_MakeVectorSwizzle4QuarterGroup(type,c,a,b,d)\
Light_MakeVectorSwizzle4QuarterGroup(type,d,a,b,c)

#define Light_MakeVectorSwizzleGroup2(type,a,b)\
Light_MakeVectorSwizzle2Group(type,a,b)

#define Light_MakeVectorSwizzleGroup3(type,a,b,c)\
Light_MakeVectorSwizzle2Group(type,a,b)\
Light_MakeVectorSwizzle2Group(type,a,c)\
Light_MakeVectorSwizzle2Group(type,b,c)\
Light_MakeVectorSwizzle3Group(type,a,b,c)

#define Light_MakeVectorSwizzleGroup4(type,a,b,c,d)\
Light_MakeVectorSwizzle2Group(type,a,b)\
Light_MakeVectorSwizzle2Group(type,a,c)\
Light_MakeVectorSwizzle2Group(type,a,d)\
Light_MakeVectorSwizzle2Group(type,b,c)\
Light_MakeVectorSwizzle2Group(type,b,d)\
Light_MakeVectorSwizzle2Group(type,c,d)\
Light_MakeVectorSwizzle3Group(type,a,b,c)\
Light_MakeVectorSwizzle3Group(type,a,b,d)\
Light_MakeVectorSwizzle3Group(type,a,c,d)\
Light_MakeVectorSwizzle3Group(type,b,c,d)\
Light_MakeVectorSwizzle4Group(type,a,b,c,d)
}
