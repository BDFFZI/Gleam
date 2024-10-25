#pragma once
#include <stdexcept>

#define VectorMemberFunction(Type,Number) \
constexpr vector(const Type value)\
{\
for (int i = 0; i < (Number); i++)\
data[i] = value;\
}\
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

    VectorMemberFunction(Type, Number)
};

template <class Type, int... Index>
struct VectorSwizzle
{
    VectorSwizzle(): address(reinterpret_cast<Type*>(this))
    {
    }
    VectorSwizzle(Type* address): address(address)
    {
    }

    Type& At(const int i)
    {
        return address[i];
    }
    const Type& At(const int i) const
    {
        return address[i];
    }

    constexpr VectorSwizzle& operator=(const vector<Type, sizeof...(Index)>& value)
    {
        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            At(indices[i]) = value[i];
        return *this;
    }
    constexpr VectorSwizzle& operator=(const VectorSwizzle& value)
    {
        if (&value == this)
            return *this;

        constexpr int indices[] = {Index...};
        for (int i = 0; i < sizeof...(Index); i++)
            At(indices[i]) = value.At(i);
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

private:
    Type* address;
};

consteval int VertexComponentMap(const std::string_view& str)
{
    if (str == "x" || str == "r") return 0;
    if (str == "y" || str == "g") return 1;
    if (str == "z" || str == "b") return 2;
    if (str == "w" || str == "a") return 3;

    throw std::out_of_range("不支持的变量名！");
}

#define PlaceSwizzle2(map,type,a,b) VectorSwizzle<type,map(#a),map(#b)> a##b;
#define PlaceSwizzle3(map,type,a,b,c) VectorSwizzle<type,map(#a),map(#b),map(#c)> a##b##c;
#define PlaceSwizzle4(map,type,a,b,c,d) VectorSwizzle<type,map(#a),map(#b),map(#c),map(#d)> a##b##c##d;

#define PlaceSwizzle2Group(map,type,a,b) \
PlaceSwizzle2(map,type,a,b)\
PlaceSwizzle2(map,type,b,a)

#define PlaceSwizzle3Group(map,type,a,b,c) \
PlaceSwizzle3(map,type,a,b,c)\
PlaceSwizzle3(map,type,a,c,b)\
PlaceSwizzle3(map,type,b,a,c)\
PlaceSwizzle3(map,type,b,c,a)\
PlaceSwizzle3(map,type,c,a,b)\
PlaceSwizzle3(map,type,c,b,a)

#define PlaceSwizzle4QuarterGroup(map,type,head,a,b,c)\
    PlaceSwizzle4(map,type,head,a,b,c)\
    PlaceSwizzle4(map,type,head,a,c,b)\
    PlaceSwizzle4(map,type,head,b,a,c)\
    PlaceSwizzle4(map,type,head,b,c,a)\
    PlaceSwizzle4(map,type,head,c,a,b)\
    PlaceSwizzle4(map,type,head,c,b,a)

#define PlaceSwizzle4Group(map,type,a,b,c,d) \
PlaceSwizzle4QuarterGroup(map,type,a,b,c,d)\
PlaceSwizzle4QuarterGroup(map,type,b,a,c,d)\
PlaceSwizzle4QuarterGroup(map,type,c,a,b,d)\
PlaceSwizzle4QuarterGroup(map,type,d,a,b,c)

#define PlaceSwizzleGroup2(map,type,a,b)\
PlaceSwizzle2Group(map,type,a,b)

#define PlaceSwizzleGroup3(map,type,a,b,c)\
PlaceSwizzle2Group(map,type,a,b)\
PlaceSwizzle2Group(map,type,a,c)\
PlaceSwizzle2Group(map,type,b,c)\
PlaceSwizzle3Group(map,type,a,b,c)

#define PlaceSwizzleGroup4(map,type,a,b,c,d)\
PlaceSwizzle2Group(map,type,a,b)\
PlaceSwizzle2Group(map,type,a,c)\
PlaceSwizzle2Group(map,type,a,d)\
PlaceSwizzle2Group(map,type,b,c)\
PlaceSwizzle2Group(map,type,b,d)\
PlaceSwizzle2Group(map,type,c,d)\
PlaceSwizzle3Group(map,type,a,b,c)\
PlaceSwizzle3Group(map,type,a,b,d)\
PlaceSwizzle3Group(map,type,a,c,d)\
PlaceSwizzle3Group(map,type,b,c,d)\
PlaceSwizzle4Group(map,type,a,b,c,d)
