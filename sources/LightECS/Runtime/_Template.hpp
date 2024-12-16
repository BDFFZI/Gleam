#pragma once
#include <cstdint>
#include <limits>

namespace Light
{


    template <int MinOrder, int MaxOrder>
    struct SystemT
    {
        static_assert(MinOrder <= MaxOrder, "最大顺序不能小于最小顺序！");
        static constexpr int32_t Order = static_cast<int32_t>((static_cast<int64_t>(MinOrder) + static_cast<int64_t>(MaxOrder)) / 2);

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
}
