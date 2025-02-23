#pragma once
#include <algorithm>
#include <cmath>
#include <format>

#include "../Math.h"
#include "Vector.h"

//模板没法触发自动类型转换，且有候选优先级较低的问题，因此只能利用宏来大批量定义函数
//VV1V：表示两个向量参与计算，并将结果存在第一个向量后返回
//VV2V：表示两个向量参与计算，并将结果存在一个新向量后返回

namespace Gleam
{
#define Gleam_MakeVectorFunction_SymbolV1V(Symbol,Type,Number)\
constexpr vector<Type, Number>& operator##Symbol##(vector<Type, Number>& a)\
{\
for (int i = 0; i < (Number); i++)\
Symbol##a.data[i];\
return a;\
}

#define Gleam_MakeVectorFunction_SymbolV1VSuf(Symbol,Type,Number)\
constexpr vector<Type, Number> operator##Symbol##(vector<Type, Number>& a,int)\
{\
vector<Type, Number> temp = a;\
for (int i = 0; i < (Number); i++)\
Symbol##a.data[i];\
return temp;\
}

#define Gleam_MakeVectorFunction_SymbolV2V(Symbol,Type,Number)\
constexpr vector<Type, Number> operator##Symbol##(const vector<Type, Number>& a)\
{\
vector<Type, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = Symbol##a.data[i];\
return result;\
}

#define Gleam_MakeVectorFunction_SymbolVV1V(Symbol,Type,Number)\
constexpr vector<Type, Number>& operator##Symbol##(vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
for (int i = 0; i < (Number); i++)\
a.data[i] Symbol b.data[i];\
return a;\
}

#define Gleam_MakeVectorFunction_SymbolVV2TV(Symbol,Type,ResultType,Number)\
constexpr vector<ResultType, Number> operator##Symbol##(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
vector<ResultType, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = a.data[i] Symbol b.data[i];\
return result;\
}
#define Gleam_MakeVectorFunction_SymbolVV2V(Symbol,Type,Number)\
Gleam_MakeVectorFunction_SymbolVV2TV(Symbol,Type,Type,Number)

#define Gleam_MakeVectorFunction_FunctionV2V(Function,Type,Number,NameSpace)\
constexpr vector<Type, Number> Function##(const vector<Type, Number>& a)\
{\
vector<Type, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = NameSpace::Function(a.data[i]);\
return result;\
}

#define Gleam_MakeVectorFunction_FunctionVV2V(Function,Type,Number)\
constexpr vector<Type, Number> Function##(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
vector<Type, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = std::Function(a.data[i],b.data[i]);\
return result;\
}

#define Gleam_MakeVectorFunction_FunctionVVV2V(Function,Type,Number)\
constexpr vector<Type, Number> Function##(const vector<Type, Number>& a, const vector<Type, Number>& b, const vector<Type, Number>& c)\
{\
vector<Type, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = std::Function(a.data[i], b.data[i], c.data[i]);\
return result;\
}

#define Gleam_MakeVectorFunction_Compare(Type,Number)\
constexpr vector<bool, Number> operator==(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
vector<bool, Number> result;\
for (int i = 0; i < (Number); i++)\
    result.data[i] = equal(a.data[i],b.data[i]);\
return result;\
}\
constexpr vector<bool, Number> operator!=(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
vector<bool, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = !equal(a.data[i],b.data[i]);\
return result;\
}\
Gleam_MakeVectorFunction_SymbolVV2TV(<,Type,bool,Number)\
Gleam_MakeVectorFunction_SymbolVV2TV(<=,Type,bool,Number)\
Gleam_MakeVectorFunction_SymbolVV2TV(>,Type,bool,Number)\
Gleam_MakeVectorFunction_SymbolVV2TV(>=,Type,bool,Number)

#define Gleam_MakeVectorFunction_All(Type,Number)\
constexpr bool all(const vector<Type, Number>& a)\
{\
    for (int i = 0; i < (Number); i++)\
        if (a.data[i] == 0)\
            return false;\
    return true;\
}
#define Gleam_MakeVectorFunction_Any(Type,Number)\
constexpr bool any(const vector<Type, Number>& a)\
{\
    for (int i = 0; i < (Number); i++)\
        if (a.data[i] != 0)\
            return true;\
    return false;\
}

#define Gleam_MakeVectorFunction_Dot(Type,Number)\
constexpr Type dot(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
Type result = 0;\
for (int i = 0; i < (Number); i++)\
result += a.data[i] * b.data[i];\
return result;\
}

#define Gleam_MakeVectorFunction_Length(Type,Number)\
constexpr Type lengthsq(const vector<Type, Number>& a)\
{\
return dot(a, a);\
}\
inline Type length(const vector<Type, Number>& a)\
{\
return ::sqrt(lengthsq(a));\
}

#define Gleam_MakeVectorFunction_Normalize(Type,Number)\
constexpr vector<Type, Number> normalize(const vector<Type, Number>& a)\
{\
return a / length(a);\
}\
constexpr vector<Type, Number> normalizesafe(const vector<Type, Number>& a,const vector<Type, Number>& defaultValue = {})\
{\
    Type len = dot(a,a);\
    return len > std::numeric_limits<float>::epsilon() ? a * rsqrt(len) : defaultValue;\
}

#define Gleam_MakeVectorFunction_Distance(Type,Number)\
inline Type distance(const vector<Type, Number>& a,const vector<Type, Number>& b)\
{\
return length(a - b);\
}

    /**
     * 线性插值
     * @tparam Type 
     * @tparam Number 
     * @param origin 起点
     * @param destination 终点
     * @param rate 插值比率
     * @return 
     */
#define Gleam_MakeVectorFunction_Lerp(Type,Number)\
constexpr vector<Type, Number> lerp(const vector<Type, Number>& origin, const vector<Type, Number>& destination, const vector<Type, Number>& rate)\
{\
    return origin + rate * (destination - origin);\
}

    /**
     * 求两个向量的夹角角度
     * @tparam Type 
     * @tparam Number 
     * @param a 
     * @param b 
     * @return 
     */
#define Gleam_MakeVectorFunction_Angle(Type,Number)\
constexpr Type angle(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
    return degrees(::acos(dot(a, b) / ::sqrt(lengthsq(a) * lengthsq(b))));\
}

    /**
     * 向量投影
     * @tparam Type 
     * @tparam Number 
     * @param v 被投影向量
     * @param n 投影到的单位法线
     * @return 
     */
#define Gleam_MakeVectorFunction_Project(Type,Number)\
constexpr vector<Type, Number> project(const vector<Type, Number>& v, const vector<Type, Number>& n)\
{\
    return n * dot(v, n);\
}

#define Gleam_MakeVectorFunctions_Integer(Type,Number)\
Gleam_MakeVectorFunction_SymbolV1V(++, Type, Number);\
Gleam_MakeVectorFunction_SymbolV1V(--, Type, Number);\
Gleam_MakeVectorFunction_SymbolV1VSuf(++, Type, Number);\
Gleam_MakeVectorFunction_SymbolV1VSuf(--, Type, Number);\
Gleam_MakeVectorFunction_SymbolV2V(-, Type, Number);\
Gleam_MakeVectorFunction_SymbolVV1V(+=, Type, Number);\
Gleam_MakeVectorFunction_SymbolVV1V(-=, Type, Number);\
Gleam_MakeVectorFunction_SymbolVV1V(*=, Type, Number);\
Gleam_MakeVectorFunction_SymbolVV1V(/=, Type, Number);\
Gleam_MakeVectorFunction_SymbolVV2V(+, Type, Number);\
Gleam_MakeVectorFunction_SymbolVV2V(-, Type, Number);\
Gleam_MakeVectorFunction_SymbolVV2V(*, Type, Number);\
Gleam_MakeVectorFunction_SymbolVV2V(/, Type, Number);\
Gleam_MakeVectorFunction_Compare(Type, Number);\
Gleam_MakeVectorFunction_All(Type, Number);\
Gleam_MakeVectorFunction_Any(Type, Number);

#define Gleam_MakeVectorFunctions_Decimals(Type,Number)\
Gleam_MakeVectorFunctions_Integer(Type,Number)\
Gleam_MakeVectorFunction_FunctionV2V(cos, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(acos, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(sin, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(asin, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(tan, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(atan, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(degrees, Type, Number,Gleam);\
Gleam_MakeVectorFunction_FunctionV2V(radians, Type, Number,Gleam);\
Gleam_MakeVectorFunction_FunctionV2V(sqrt, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(rsqrt, Type, Number,Gleam);\
Gleam_MakeVectorFunction_FunctionV2V(abs, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(round, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(trunc, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(ceil, Type, Number, );\
Gleam_MakeVectorFunction_FunctionV2V(floor, Type, Number, );\
Gleam_MakeVectorFunction_FunctionVV2V(fmod, Type, Number);\
Gleam_MakeVectorFunction_FunctionVV2V(pow, Type, Number);\
Gleam_MakeVectorFunction_FunctionVV2V(max, Type, Number);\
Gleam_MakeVectorFunction_FunctionVV2V(min, Type, Number);\
Gleam_MakeVectorFunction_FunctionVVV2V(clamp, Type, Number);\
Gleam_MakeVectorFunction_Dot(Type, Number);\
Gleam_MakeVectorFunction_Length(Type, Number);\
Gleam_MakeVectorFunction_Normalize(Type, Number);\
Gleam_MakeVectorFunction_Distance(Type, Number);\
Gleam_MakeVectorFunction_Lerp(Type, Number);\
Gleam_MakeVectorFunction_Angle(Type, Number);\
Gleam_MakeVectorFunction_Project(Type, Number);


    /**
     * 三维向量叉乘
     * @param Type 
     */
#define Gleam_MakeVectorFunction_Cross3(Type)\
constexpr vector<Type, 3> cross(const vector<Type, 3>& a, const vector<Type, 3>& b)\
{\
return {\
a.y * b.z - a.z * b.y,\
-(a.x * b.z - a.z * b.x),\
a.x * b.y - a.y * b.x\
};\
}

    /**
     * 三维向量绕轴旋转
     * @tparam Type 
     * @param v 被旋转的向量
     * @param n 作为旋转轴的单位向量
     * @param angle 旋转的角度
     * @return 
     */
#define Gleam_MakeVectorFunction_Rotate3(Type)\
constexpr vector<Type, 3> rotate(const vector<Type, 3>& v, const vector<Type, 3>& n, const Type angle)\
{\
Type rad = radians(angle);\
vector<Type, 3> parallel = project(v, n);\
vector<Type, 3> perpendicular = v - parallel;\
return perpendicular * ::cos(rad) + cross(n, perpendicular) * ::sin(rad) + parallel;\
}

#define Gleam_MakeVectorFunctions_Vector3Extra(Type)\
Gleam_MakeVectorFunction_Cross3(Type)\
Gleam_MakeVectorFunction_Rotate3(Type)

#define Gleam_MakeVectorFunction_ToString(Type,Number,Format)\
inline std::string to_string(const vector<Type, Number>& a)\
{\
std::string str = "(";\
for (int i = 0; i < (Number); i++)\
str += std::format("{" Format "},", a.data[i]);\
str.pop_back();\
return str + ")";\
}

    Gleam_MakeVectorFunctions_Decimals(float, 2)
    Gleam_MakeVectorFunctions_Decimals(float, 3)
    Gleam_MakeVectorFunctions_Decimals(float, 4)
    Gleam_MakeVectorFunctions_Vector3Extra(float)
    Gleam_MakeVectorFunction_ToString(float, 2, ":f")
    Gleam_MakeVectorFunction_ToString(float, 3, ":f")
    Gleam_MakeVectorFunction_ToString(float, 4, ":f")

    Gleam_MakeVectorFunctions_Integer(int, 2)
    Gleam_MakeVectorFunctions_Integer(int, 3)
    Gleam_MakeVectorFunctions_Integer(int, 4)
    Gleam_MakeVectorFunction_ToString(int, 2, "")
    Gleam_MakeVectorFunction_ToString(int, 3, "")
    Gleam_MakeVectorFunction_ToString(int, 4, "")

    Gleam_MakeVectorFunction_All(bool, 2)
    Gleam_MakeVectorFunction_All(bool, 3)
    Gleam_MakeVectorFunction_All(bool, 4)
    Gleam_MakeVectorFunction_Any(bool, 2)
    Gleam_MakeVectorFunction_Any(bool, 3)
    Gleam_MakeVectorFunction_Any(bool, 4)
}