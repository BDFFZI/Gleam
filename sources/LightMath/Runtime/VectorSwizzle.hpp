#pragma once
#include <stdexcept>

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

template <class Type>
struct MemberVectorSwizzle
{
    Type& At(const int i)
    {
        return reinterpret_cast<Type*>(this)[i];
    }
    const Type& At(const int i) const
    {
        return reinterpret_cast<const Type*>(this)[i];
    }
};

template <class Type>
struct VariableVectorSwizzle
{
    Type* ptr;
    
    Type& At(const int i)
    {
        return ptr[i];
    }
};


template <class Base, class Type, int... Index>
struct VectorSwizzle : Base
{
    constexpr VectorSwizzle& operator=(const vector<Type, sizeof...(Index)>& value)
    {
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            At(indices[i]) = value[i];
        return *this;
    }
    constexpr operator vector<Type, sizeof...(Index)>() const
    {
        vector<Type, sizeof...(Index)> result;
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            result[i] = At(indices[i]);
        return result;
    }

    constexpr VectorSwizzle& operator+=(const vector<Type, sizeof...(Index)>& value)
    {
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            At(indices[i]) += value[i];
        return *this;
    }
    constexpr VectorSwizzle& operator-=(const vector<Type, sizeof...(Index)>& value)
    {
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            At(indices[i]) -= value[i];
        return *this;
    }
    constexpr VectorSwizzle& operator*=(const vector<Type, sizeof...(Index)>& value)
    {
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            At(indices[i]) *= value[i];
        return *this;
    }
    constexpr VectorSwizzle& operator/=(const vector<Type, sizeof...(Index)>& value)
    {
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            At(indices[i]) /= value[i];
        return *this;
    }
    constexpr VectorSwizzle& operator++()
    {
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            ++At(indices[i]);
        return *this;
    }
    constexpr VectorSwizzle& operator--()
    {
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            ++At(indices[i]);
        return *this;
    }
    constexpr vector<Type, sizeof...(Index)> operator++(int)
    {
        vector<Type, sizeof...(Index)> temp = *this;
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            ++At(indices[i]);
        return temp;
    }
    constexpr vector<Type, sizeof...(Index)> operator--(int)
    {
        vector<Type, sizeof...(Index)> temp = *this;
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            --At(indices[i]);
        return temp;
    }
};

consteval int VertexComponentMap(const std::string_view& str)
{
    if (str == "x" || str == "r") return 0;
    if (str == "y" || str == "g") return 1;
    if (str == "z" || str == "b") return 2;
    if (str == "w" || str == "a") return 3;

    throw std::out_of_range("不支持的变量名！");
}

#define MakeSwizzle2(map,type,a,b) VectorSwizzle<MemberVectorSwizzle<type>,type,map(#a),map(#b)> a##b;
#define MakeSwizzle3(map,type,a,b,c) VectorSwizzle<MemberVectorSwizzle<type>,type,map(#a),map(#b),map(#c)> a##b##c;
#define MakeSwizzle4(map,type,a,b,c,d) VectorSwizzle<MemberVectorSwizzle<type>,type,map(#a),map(#b),map(#c),map(#d)> a##b##c##d;

#define MakeSwizzle2Group(map,type,a,b) \
MakeSwizzle2(map,type,a,b)\
MakeSwizzle2(map,type,b,a)

#define MakeSwizzle3Group(map,type,a,b,c) \
MakeSwizzle3(map,type,a,b,c)\
MakeSwizzle3(map,type,a,c,b)\
MakeSwizzle3(map,type,b,a,c)\
MakeSwizzle3(map,type,b,c,a)\
MakeSwizzle3(map,type,c,a,b)\
MakeSwizzle3(map,type,c,b,a)

#define MakeSwizzle4QuarterGroup(map,type,head,a,b,c)\
    MakeSwizzle4(map,type,head,a,b,c)\
    MakeSwizzle4(map,type,head,a,c,b)\
    MakeSwizzle4(map,type,head,b,a,c)\
    MakeSwizzle4(map,type,head,b,c,a)\
    MakeSwizzle4(map,type,head,c,a,b)\
    MakeSwizzle4(map,type,head,c,b,a)

#define MakeSwizzle4Group(map,type,a,b,c,d) \
MakeSwizzle4QuarterGroup(map,type,a,b,c,d)\
MakeSwizzle4QuarterGroup(map,type,b,a,c,d)\
MakeSwizzle4QuarterGroup(map,type,c,a,b,d)\
MakeSwizzle4QuarterGroup(map,type,d,a,b,c)

#define MakeSwizzleGroup2(map,type,a,b)\
MakeSwizzle2Group(map,type,a,b)

#define MakeSwizzleGroup3(map,type,a,b,c)\
MakeSwizzle2Group(map,type,a,b)\
MakeSwizzle2Group(map,type,a,c)\
MakeSwizzle2Group(map,type,b,c)\
MakeSwizzle3Group(map,type,a,b,c)

#define MakeSwizzleGroup4(map,type,a,b,c,d)\
MakeSwizzle2Group(map,type,a,b)\
MakeSwizzle2Group(map,type,a,c)\
MakeSwizzle2Group(map,type,a,d)\
MakeSwizzle2Group(map,type,b,c)\
MakeSwizzle2Group(map,type,b,d)\
MakeSwizzle2Group(map,type,c,d)\
MakeSwizzle3Group(map,type,a,b,c)\
MakeSwizzle3Group(map,type,a,b,d)\
MakeSwizzle3Group(map,type,a,c,d)\
MakeSwizzle3Group(map,type,b,c,d)\
MakeSwizzle4Group(map,type,a,b,c,d)
