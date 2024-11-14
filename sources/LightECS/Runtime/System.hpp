#pragma once
#include <limits>

constexpr float MinSystemOrder = std::numeric_limits<float>::min();
constexpr float MaxSystemOrder = std::numeric_limits<float>::max();

template <float MinOrder, float MaxOrder>
struct System
{
    static constexpr float Order = (MinOrder + MaxOrder) / 2;

    static void Start()
    {
    }
    static void Update()
    {
    }
    static void Stop()
    {
    }
};
