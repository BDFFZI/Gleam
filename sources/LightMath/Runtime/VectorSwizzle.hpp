#pragma once

#define MakeVectorMemberFunctions(Type,Number) \
constexpr vector() { };\
constexpr vector(const Type value)\
{\
for (int i = 0; i < (Number); i++)\
data[i] = value;\
}

#define MakeVectorMemberFunctions_Indexer(Type)\
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

    MakeVectorMemberFunctions(Type, Number)
    MakeVectorMemberFunctions_Indexer(Type)
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

#define MakeVectorSwizzle2(type,a,b) VectorSwizzle<type,a##i,b##i> a##b;
#define MakeVectorSwizzle3(type,a,b,c) VectorSwizzle<type,a##i,b##i,c##i> a##b##c;
#define MakeVectorSwizzle4(type,a,b,c,d) VectorSwizzle<type,a##i,b##i,c##i,d##i> a##b##c##d;

#define MakeVectorSwizzle2Group(type,a,b) \
MakeVectorSwizzle2(type,a,b)\
MakeVectorSwizzle2(type,b,a)

#define MakeVectorSwizzle3Group(type,a,b,c) \
MakeVectorSwizzle3(type,a,b,c)\
MakeVectorSwizzle3(type,a,c,b)\
MakeVectorSwizzle3(type,b,a,c)\
MakeVectorSwizzle3(type,b,c,a)\
MakeVectorSwizzle3(type,c,a,b)\
MakeVectorSwizzle3(type,c,b,a)

#define MakeVectorSwizzle4QuarterGroup(type,head,a,b,c)\
    MakeVectorSwizzle4(type,head,a,b,c)\
    MakeVectorSwizzle4(type,head,a,c,b)\
    MakeVectorSwizzle4(type,head,b,a,c)\
    MakeVectorSwizzle4(type,head,b,c,a)\
    MakeVectorSwizzle4(type,head,c,a,b)\
    MakeVectorSwizzle4(type,head,c,b,a)

#define MakeVectorSwizzle4Group(type,a,b,c,d) \
MakeVectorSwizzle4QuarterGroup(type,a,b,c,d)\
MakeVectorSwizzle4QuarterGroup(type,b,a,c,d)\
MakeVectorSwizzle4QuarterGroup(type,c,a,b,d)\
MakeVectorSwizzle4QuarterGroup(type,d,a,b,c)

#define MakeVectorSwizzleGroup2(type,a,b)\
MakeVectorSwizzle2Group(type,a,b)

#define MakeVectorSwizzleGroup3(type,a,b,c)\
MakeVectorSwizzle2Group(type,a,b)\
MakeVectorSwizzle2Group(type,a,c)\
MakeVectorSwizzle2Group(type,b,c)\
MakeVectorSwizzle3Group(type,a,b,c)

#define MakeVectorSwizzleGroup4(type,a,b,c,d)\
MakeVectorSwizzle2Group(type,a,b)\
MakeVectorSwizzle2Group(type,a,c)\
MakeVectorSwizzle2Group(type,a,d)\
MakeVectorSwizzle2Group(type,b,c)\
MakeVectorSwizzle2Group(type,b,d)\
MakeVectorSwizzle2Group(type,c,d)\
MakeVectorSwizzle3Group(type,a,b,c)\
MakeVectorSwizzle3Group(type,a,b,d)\
MakeVectorSwizzle3Group(type,a,c,d)\
MakeVectorSwizzle3Group(type,b,c,d)\
MakeVectorSwizzle4Group(type,a,b,c,d)
