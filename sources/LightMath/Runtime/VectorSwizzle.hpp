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
            ++At(indices[i]);
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
};

#define MakeVectorSwizzle2(map,type,a,b) VectorSwizzle<type,a##i,b##i> a##b;
#define MakeVectorSwizzle3(map,type,a,b,c) VectorSwizzle<type,a,b,c> a##b##c;
#define MakeVectorSwizzle4(map,type,a,b,c,d) VectorSwizzle<type,a,b,c,d> a##b##c##d;

#define MakeVectorSwizzle2Group(map,type,a,b) \
MakeVectorSwizzle2(map,type,a,b)\
MakeVectorSwizzle2(map,type,b,a)

#define MakeVectorSwizzle3Group(map,type,a,b,c) \
MakeVectorSwizzle3(map,type,a,b,c)\
MakeVectorSwizzle3(map,type,a,c,b)\
MakeVectorSwizzle3(map,type,b,a,c)\
MakeVectorSwizzle3(map,type,b,c,a)\
MakeVectorSwizzle3(map,type,c,a,b)\
MakeVectorSwizzle3(map,type,c,b,a)

#define MakeVectorSwizzle4QuarterGroup(map,type,head,a,b,c)\
    MakeVectorSwizzle4(map,type,head,a,b,c)\
    MakeVectorSwizzle4(map,type,head,a,c,b)\
    MakeVectorSwizzle4(map,type,head,b,a,c)\
    MakeVectorSwizzle4(map,type,head,b,c,a)\
    MakeVectorSwizzle4(map,type,head,c,a,b)\
    MakeVectorSwizzle4(map,type,head,c,b,a)

#define MakeVectorSwizzle4Group(map,type,a,b,c,d) \
MakeVectorSwizzle4QuarterGroup(map,type,a,b,c,d)\
MakeVectorSwizzle4QuarterGroup(map,type,b,a,c,d)\
MakeVectorSwizzle4QuarterGroup(map,type,c,a,b,d)\
MakeVectorSwizzle4QuarterGroup(map,type,d,a,b,c)

#define MakeVectorSwizzleGroup2(map,type,a,b)\
MakeVectorSwizzle2Group(map,type,a,b)

#define MakeVectorSwizzleGroup3(map,type,a,b,c)\
MakeVectorSwizzle2Group(map,type,a,b)\
MakeVectorSwizzle2Group(map,type,a,c)\
MakeVectorSwizzle2Group(map,type,b,c)\
MakeVectorSwizzle3Group(map,type,a,b,c)

#define MakeVectorSwizzleGroup4(map,type,a,b,c,d)\
MakeVectorSwizzle2Group(map,type,a,b)\
MakeVectorSwizzle2Group(map,type,a,c)\
MakeVectorSwizzle2Group(map,type,a,d)\
MakeVectorSwizzle2Group(map,type,b,c)\
MakeVectorSwizzle2Group(map,type,b,d)\
MakeVectorSwizzle2Group(map,type,c,d)\
MakeVectorSwizzle3Group(map,type,a,b,c)\
MakeVectorSwizzle3Group(map,type,a,b,d)\
MakeVectorSwizzle3Group(map,type,a,c,d)\
MakeVectorSwizzle3Group(map,type,b,c,d)\
MakeVectorSwizzle4Group(map,type,a,b,c,d)