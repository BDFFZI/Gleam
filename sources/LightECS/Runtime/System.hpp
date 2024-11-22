#pragma once
#include <cstdint>
#include <limits>

template <int32_t Order_V>
struct SystemOrder
{
    static constexpr int32_t Order = Order_V;
};
struct MinSystemOrder : SystemOrder<std::numeric_limits<int32_t>::lowest()>
{
};
struct MaxSystemOrder : SystemOrder<std::numeric_limits<int32_t>::max()>
{
};

template <class MinOrder, class MaxOrder>
struct System
{
    static_assert(MinOrder::Order <= MaxOrder::Order, "最大顺序不能小于最小顺序！");
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
