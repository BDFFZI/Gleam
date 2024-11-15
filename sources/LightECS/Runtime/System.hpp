#pragma once
#include <limits>

struct MinSystemOrder
{
    static constexpr float Order = std::numeric_limits<float>::min();
};
struct MaxSystemOrder
{
    static constexpr float Order = std::numeric_limits<float>::max();
};

template <class MinOrder, class MaxOrder>
struct System
{
    static constexpr float Order = (MinOrder::Order + MaxOrder::Order) / 2;

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

