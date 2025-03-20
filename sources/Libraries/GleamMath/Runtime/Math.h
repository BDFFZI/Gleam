#pragma once
#include <algorithm>
#include <numbers>
#include <cmath>

namespace Gleam
{
    inline float cos(const float value)
    {
        return std::cos(value);
    }
    inline float sin(const float value)
    {
        return std::sin(value);
    }
    inline float tan(const float value)
    {
        return std::tan(value);
    }
    inline float acos(const float value)
    {
        return std::acos(value);
    }
    inline float asin(const float value)
    {
        return std::asin(value);
    }
    inline float atan(const float value)
    {
        return std::atan(value);
    }
    inline float sign(const float value)
    {
        return value > 0 ? 1.0f : value < 0.0f ? -1.0f : 0.0f;
    }
    /**
     * 类似取模，但会处理负数情况，保证结果永远在0到length之间
     *
     * 实现方法：
     * 1. 获取t相对length取余后的带符号整数
     * 2. 该整数始终小于等于t，将t减去该值即可求出t相对length的无符号余数
     * @param t 
     * @param length 
     * @return 
     */
    inline float Repeat(const float t, const float length)
    {
        return std::clamp(t - std::floor(t / length) * length, 0.0f, length);
    }
    /**
     * 将t限制在0到length间来回跳动
     *
     * 实现方法：
     * 1. t相对length的倍数奇偶性决定了跳动的方向，故先对t无符号取模，获取其在0到2*length的值，其他区域都是该区间的重复。
     * 2. 直接将t减去其与length的距离即可使length左右两边的数发生弹跳。
     * @param t 
     * @param length 
     * @return 
     */
    inline float PingPong(float t, const float length)
    {
        t = Repeat(t, length * 2);
        return length - std::abs(t - length);
    }

    template <typename Type>
        requires std::is_arithmetic_v<Type>
    constexpr Type max(const Type a, const Type b) { return std::max(a, b); }

    template <typename Type>
        requires std::is_arithmetic_v<Type>
    constexpr Type min(const Type a, const Type b) { return std::min(a, b); }

    template <typename Type>
        requires std::is_arithmetic_v<Type>
    constexpr Type clamp(const Type a, const Type min, const Type max) { return std::clamp(a, min, max); }

    template <typename Type>
        requires std::is_arithmetic_v<Type>
    constexpr Type rsqrt(const Type a) { return 1 / static_cast<Type>(sqrt(a)); }

    /**
     * 弧度转角度
     * @param radian 
     * @return 
     */
    template <typename Type>
        requires std::is_arithmetic_v<Type>
    constexpr Type degrees(const Type radian)
    {
        constexpr Type Rad2Deg = Type(180) / std::numbers::pi_v<Type>;
        return radian * Rad2Deg;
    }
    /**
     * 角度转弧度
     * @param degree 
     * @return 
     */
    template <typename Type>
        requires std::is_arithmetic_v<Type>
    constexpr Type radians(const Type degree)
    {
        constexpr Type Deg2Rad = std::numbers::pi_v<Type> / Type(180);
        return degree * Deg2Rad;
    }

    template <typename Type>
        requires std::is_arithmetic_v<Type>
    constexpr bool equal(const Type a, const Type b)
    {
        return abs(a - b) <= std::numeric_limits<Type>::epsilon();
    }
}