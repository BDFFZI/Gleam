#pragma once
#include <algorithm>
#include <numbers>
#include <cmath>

#include "Vector.hpp"

template <typename Type>
constexpr Type max(const Type a, const Type b) { return std::max(a, b); }
template <typename Type>
constexpr Type min(const Type a, const Type b) { return std::min(a, b); }
template <typename Type>
constexpr Type rsqrt(const Type a) { return 1 / sqrtf(a); }

/**
 * 弧度转角度
 * @param radian 
 * @return 
 */
template <typename Type>
constexpr Type degrees(const Type radian)
{
    constexpr Type Rad2Deg = 180.0f / std::numbers::pi_v<Type>;
    return radian * Rad2Deg;
}
/**
 * 角度转弧度
 * @param degree 
 * @return 
 */
template <typename Type>
constexpr float radians(const Type degree)
{
    constexpr Type Deg2Rad = std::numbers::pi_v<Type> / 180.0f;
    return degree * Deg2Rad;
}

template <class Type, int Number>
constexpr vector<Type, Number> operator+(const vector<Type, Number>& a, const Type b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = a[i] + b;
    return result;
}
template <class Type, int Number>
constexpr vector<Type, Number> operator+(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = a[i] + b[i];
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> operator-(const vector<Type, Number>& a, const Type b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = a[i] - b;
    return result;
}
template <class Type, int Number>
constexpr vector<Type, Number> operator-(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = a[i] - b[i];
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> operator*(const vector<Type, Number>& a, const Type b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = a[i] * b;
    return result;
}
template <class Type, int Number>
constexpr vector<Type, Number> operator*(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = a[i] * b[i];
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> operator/(const vector<Type, Number>& a, const Type b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = a[i] / b;
    return result;
}
template <class Type, int Number>
constexpr vector<Type, Number> operator/(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = a[i] / b[i];
    return result;
}

template <class Type, int Number>
constexpr vector<bool, Number> operator==(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    vector<bool, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = abs(a[i] - b[i]) <= std::numeric_limits<Type>::epsilon();
    return result;
}

template <class Type, int Number>
constexpr vector<bool, Number> operator!=(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    return !operator==<Type, Number>(a, b);
}

template <class Type, int Number>
constexpr bool all(const vector<Type, Number>& a)
{
    for (int i = 0; i < Number; i++)
        if (a[i] == 0)
            return false;

    return true;
}

template <class Type, int Number>
constexpr vector<Type, Number> fmod(const vector<Type, Number>& a, const Type b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = fmod(a[i], b);
    return result;
}
template <class Type, int Number>
constexpr vector<Type, Number> fmod(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = fmod(a[i], b[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> pow(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = pow(a[i], b[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> cos(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = cos(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> acos(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = acos(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> sin(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = sin(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> asin(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = asin(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> tan(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = tan(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> atan(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = atan(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> degrees(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = degrees(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> radians(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = radians(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> sqrt(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = sqrt(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> rsqrt(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = rsqrt(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> abs(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = abs(a[i]);
    return result;
}

/**
 * 将小数四舍五入为整数
 * @tparam Type 
 * @tparam Number 
 * @param a 
 * @return 
 */
template <class Type, int Number>
constexpr vector<Type, Number> round(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = round(a[i]);
    return result;
}

/**
 * 将小数截断为整数
 * @tparam Type 
 * @tparam Number 
 * @param a 
 * @return 
 */
template <class Type, int Number>
constexpr vector<Type, Number> trunc(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = trunc(a[i]);
    return result;
}

/**
 * 将小数向上取整
 * @tparam Type 
 * @tparam Number 
 * @param a 
 * @return 
 */
template <class Type, int Number>
constexpr vector<Type, Number> ceil(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = ceil(a[i]);
    return result;
}

/**
 * 将小数向下取整
 * @tparam Type 
 * @tparam Number 
 * @param a 
 * @return 
 */
template <class Type, int Number>
constexpr vector<Type, Number> floor(const vector<Type, Number>& a)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = floor(a[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> max(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = max(a[i], b[i]);
    return result;
}

template <class Type, int Number>
constexpr vector<Type, Number> min(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = min(a[i], b[i]);
    return result;
}

/**
 * 将小数向下取整
 * @tparam Type 
 * @tparam Number 
 * @param a
 * @param min
 * @param max 
 * @return 
 */
template <class Type, int Number>
constexpr vector<Type, Number> clamp(const vector<Type, Number>& a, const vector<Type, Number>& min, const vector<Type, Number>& max)
{
    vector<Type, Number> result;
    for (int i = 0; i < Number; i++)
        result[i] = clamp(a[i], min[i], max[i]);
    return result;
}

/**
 * 线性插值
 * @tparam Type 
 * @tparam Number 
 * @param a 起点
 * @param b 终点
 * @param rate 插值比率
 * @return 
 */
template <class Type, int Number>
constexpr vector<Type, Number> lerp(const vector<Type, Number>& a, const vector<Type, Number>& b, const float rate)
{
    return a + vector<Type, Number>(rate) * (b - a);
}

template <class Type, int Number>
constexpr Type dot(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    Type result = 0;
    for (int i = 0; i < Number; i++)
        result += a[i] * b[i];
    return result;
}

template <class Type>
constexpr vector<Type, 3> cross(const vector<Type, 3>& a, const vector<Type, 3>& b)
{
    return {
        a.y * b.z - a.z * b.y,
        -(a.x * b.z - a.z * b.x),
        a.x * b.y - a.y * b.x
    };
}

template <class Type, int Number>
constexpr Type length_sq(const vector<Type, Number>& a)
{
    return dot(a, a);
}

template <class Type, int Number>
constexpr Type length(const vector<Type, Number>& a)
{
    return sqrt(length_sq(a));
}

/**
 * 求两个向量的夹角角度
 * @tparam Type 
 * @tparam Number 
 * @param a 
 * @param b 
 * @return 
 */
template <class Type, int Number>
constexpr Type angle(const vector<Type, Number>& a, const vector<Type, Number>& b)
{
    return degrees(acos(dot(a, b) / sqrt(length_sq(a) * length_sq(b))));
}

template <class Type, int Number>
constexpr vector<Type, Number> normalize(const vector<Type, Number>& a)
{
    return a / length(a);
}

/**
 * 向量投影
 * @tparam Type 
 * @tparam Number 
 * @param v 被投影向量
 * @param n 投影到的单位法线
 * @return 
 */
template <class Type, int Number>
constexpr vector<Type, Number> project(const vector<Type, Number>& v, const vector<Type, Number>& n)
{
    return n * dot(v, n);
}

/**
 * 向量绕轴旋转
 * @tparam Type 
 * @tparam Number 
 * @param v 被旋转的向量
 * @param n 作为旋转轴的单位向量
 * @param angle 旋转的角度
 * @return 
 */
template <class Type, int Number>
constexpr vector<Type, Number> rotate(const vector<Type, Number>& v, const vector<Type, Number>& n, const Type angle)
{
    Type rad = radians(angle);
    vector<Type, Number> parallel = project(v, n);
    vector<Type, Number> perpendicular = v - parallel;
    return perpendicular * cos(rad) + cross(n, perpendicular) * sin(rad) + parallel;
}
