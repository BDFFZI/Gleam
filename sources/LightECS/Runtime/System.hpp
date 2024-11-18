#pragma once
#include <cstdint>
#include <limits>

struct MinSystemOrder
{
    static constexpr int32_t Order = std::numeric_limits<int32_t>::lowest();
};
struct MaxSystemOrder
{
    static constexpr int32_t Order = std::numeric_limits<int32_t>::max();
};

template <class MinOrder, class MaxOrder>
struct System
{
    static constexpr int32_t Order = static_cast<int32_t>((static_cast<int64_t>(MinOrder::Order) + static_cast<int64_t>(MaxOrder::Order)) / 2);

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
