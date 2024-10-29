#pragma once
#include <algorithm>
#include <cmath>
#include <format>

#include "Math.hpp"
#include "Vector.hpp"

//模板没法触发自动类型转换，且有候选优先级较低的问题，因此只能利用宏来大批量定义函数

#define MakeVectorFunction_SymbolV1V(Symbol,Type,Number)\
constexpr vector<Type, Number>& operator##Symbol##(vector<Type, Number>& a)\
{\
for (int i = 0; i < (Number); i++)\
Symbol##a.data[i];\
return a;\
}

#define MakeVectorFunction_SymbolV1VSuf(Symbol,Type,Number)\
constexpr vector<Type, Number> operator##Symbol##(vector<Type, Number>& a,int)\
{\
vector<Type, Number> temp = a;\
for (int i = 0; i < (Number); i++)\
Symbol##a.data[i];\
return temp;\
}

#define MakeVectorFunction_SymbolVV1V(Symbol,Type,Number)\
constexpr vector<Type, Number>& operator##Symbol##(vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
for (int i = 0; i < (Number); i++)\
a.data[i] Symbol b.data[i];\
return a;\
}

#define MakeVectorFunction_SymbolVV2V(Symbol,Type,Number)\
constexpr vector<Type, Number> operator##Symbol##(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
    vector<Type, Number> result;\
    for (int i = 0; i < (Number); i++)\
        result.data[i] = a.data[i] Symbol b.data[i];\
    return result;\
}

#define MakeVectorFunction_FunctionV2V(Function,Type,Number)\
constexpr vector<Type, Number> Function##(const vector<Type, Number>& a)\
{\
vector<Type, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = Function(a.data[i]);\
return result;\
}

#define MakeVectorFunction_FunctionVV2V(Function,Type,Number)\
constexpr vector<Type, Number> Function##(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
vector<Type, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = std::Function(a.data[i],b.data[i]);\
return result;\
}

#define MakeVectorFunction_FunctionVVV2V(Function,Type,Number)\
constexpr vector<Type, Number> Function##(const vector<Type, Number>& a, const vector<Type, Number>& b, const vector<Type, Number>& c)\
{\
vector<Type, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = std::Function(a.data[i], b.data[i], c.data[i]);\
return result;\
}

#define MakeVectorFunction_Equal(Type,Number)\
constexpr vector<bool, Number> operator==(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
vector<bool, Number> result;\
for (int i = 0; i < (Number); i++)\
    result.data[i] = abs(a.data[i] - b.data[i]) <= std::numeric_limits<Type>::epsilon();\
return result;\
}\
constexpr vector<bool, Number> operator!=(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
vector<bool, Number> result;\
for (int i = 0; i < (Number); i++)\
result.data[i] = abs(a.data[i] - b.data[i]) > std::numeric_limits<Type>::epsilon();\
return result;\
}

#define MakeVectorFunction_All(Type,Number)\
constexpr bool all(const vector<Type, Number>& a)\
{\
    for (int i = 0; i < (Number); i++)\
        if (a.data[i] == 0)\
            return false;\
    return true;\
}

#define MakeVectorFunction_Dot(Type,Number)\
constexpr Type dot(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
Type result = 0;\
for (int i = 0; i < (Number); i++)\
result += a.data[i] * b.data[i];\
return result;\
}

#define MakeVectorFunction_Length(Type,Number)\
constexpr Type lengthsq(const vector<Type, Number>& a)\
{\
return dot(a, a);\
}\
inline Type length(const vector<Type, Number>& a)\
{\
return sqrt(lengthsq(a));\
}

#define MakeVectorFunction_Normalize(Type,Number)\
constexpr vector<Type, Number> normalize(const vector<Type, Number>& a)\
{\
return a / length(a);\
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
#define MakeVectorFunction_Lerp(Type,Number)\
constexpr vector<Type, Number> lerp(const vector<Type, Number>& origin, const vector<Type, Number>& destination, const Type rate)\
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
#define MakeVectorFunction_Angle(Type,Number)\
constexpr Type angle(const vector<Type, Number>& a, const vector<Type, Number>& b)\
{\
    return degrees(acos(dot(a, b) / sqrt(lengthsq(a) * lengthsq(b))));\
}

/**
 * 向量投影
 * @tparam Type 
 * @tparam Number 
 * @param v 被投影向量
 * @param n 投影到的单位法线
 * @return 
 */
#define MakeVectorFunction_Project(Type,Number)\
constexpr vector<Type, Number> project(const vector<Type, Number>& v, const vector<Type, Number>& n)\
{\
    return n * dot(v, n);\
}

#define MakeVectorFunctions(Type,Number)\
MakeVectorFunction_SymbolV1V(++, Type, Number);\
MakeVectorFunction_SymbolV1V(--, Type, Number);\
MakeVectorFunction_SymbolV1VSuf(++, Type, Number);\
MakeVectorFunction_SymbolV1VSuf(--, Type, Number);\
MakeVectorFunction_SymbolVV1V(+=, Type, Number);\
MakeVectorFunction_SymbolVV1V(-=, Type, Number);\
MakeVectorFunction_SymbolVV1V(*=, Type, Number);\
MakeVectorFunction_SymbolVV1V(/=, Type, Number);\
MakeVectorFunction_SymbolVV2V(+, Type, Number);\
MakeVectorFunction_SymbolVV2V(-, Type, Number);\
MakeVectorFunction_SymbolVV2V(*, Type, Number);\
MakeVectorFunction_SymbolVV2V(/, Type, Number);\
MakeVectorFunction_FunctionV2V(cos, Type, Number);\
MakeVectorFunction_FunctionV2V(acos, Type, Number);\
MakeVectorFunction_FunctionV2V(sin, Type, Number);\
MakeVectorFunction_FunctionV2V(asin, Type, Number);\
MakeVectorFunction_FunctionV2V(tan, Type, Number);\
MakeVectorFunction_FunctionV2V(atan, Type, Number);\
MakeVectorFunction_FunctionV2V(degrees, Type, Number);\
MakeVectorFunction_FunctionV2V(radians, Type, Number);\
MakeVectorFunction_FunctionV2V(sqrt, Type, Number);\
MakeVectorFunction_FunctionV2V(rsqrt, Type, Number);\
MakeVectorFunction_FunctionV2V(abs, Type, Number);\
MakeVectorFunction_FunctionV2V(round, Type, Number);\
MakeVectorFunction_FunctionV2V(trunc, Type, Number);\
MakeVectorFunction_FunctionV2V(ceil, Type, Number);\
MakeVectorFunction_FunctionV2V(floor, Type, Number);\
MakeVectorFunction_FunctionVV2V(fmod, Type, Number);\
MakeVectorFunction_FunctionVV2V(pow, Type, Number);\
MakeVectorFunction_FunctionVV2V(max, Type, Number);\
MakeVectorFunction_FunctionVV2V(min, Type, Number);\
MakeVectorFunction_FunctionVVV2V(clamp, Type, Number);\
MakeVectorFunction_Equal(Type, Number);\
MakeVectorFunction_All(Type, Number);\
MakeVectorFunction_All(bool, Number)\
MakeVectorFunction_Dot(Type, Number);\
MakeVectorFunction_Length(Type, Number);\
MakeVectorFunction_Normalize(Type, Number);\
MakeVectorFunction_Lerp(Type, Number);\
MakeVectorFunction_Angle(Type, Number);\
MakeVectorFunction_Project(Type, Number);


/**
 * 三维向量叉乘
 * @param Type 
 */
#define MakeVectorFunction_Cross3(Type)\
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
#define MakeVectorFunction_Rotate3(Type)\
constexpr vector<Type, 3> rotate(const vector<Type, 3>& v, const vector<Type, 3>& n, const Type angle)\
{\
Type rad = radians(angle);\
vector<Type, 3> parallel = project(v, n);\
vector<Type, 3> perpendicular = v - parallel;\
return perpendicular * cos(rad) + cross(n, perpendicular) * sin(rad) + parallel;\
}

#define MakeVectorFunctions_Vector3Extra(Type)\
MakeVectorFunction_Cross3(Type)\
MakeVectorFunction_Rotate3(Type)

#define MakeVectorFunction_ToString(Type,Number)\
std::string to_string(const vector<Type, Number>& a)\
{\
std::string str = "(";\
for (int i = 0; i < (Number); i++)\
str += std::format("{:f},", a.data[i]);\
str.pop_back();\
return str + ")";\
}

MakeVectorFunctions(float, 2)
MakeVectorFunctions(float, 3)
MakeVectorFunctions(float, 4)
MakeVectorFunctions_Vector3Extra(float)

MakeVectorFunction_ToString(float, 2)
MakeVectorFunction_ToString(float, 3)
MakeVectorFunction_ToString(float, 4)
