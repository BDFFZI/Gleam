#pragma once
#include <cfloat>
#include <numbers>

inline constexpr float Pi = std::numbers::pi_v<float>;
inline constexpr float Deg2Rad = Pi / 180.0f;
inline constexpr float Rad2Deg = 180.0f / Pi;
inline constexpr float SqrEPSILON = FLT_EPSILON * FLT_EPSILON;

/**
 * 
 * @param seed 用于存储合并后的哈希值
 * @param hash 
 */
void CombineHash(size_t& seed, size_t hash);
