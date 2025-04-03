#pragma once
#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>

namespace Gleam
{
    static constexpr int32_t SystemMinOrder = std::numeric_limits<int32_t>::lowest();
    static constexpr int32_t SystemMaxOrder = std::numeric_limits<int32_t>::max();
    static constexpr int32_t SystemMidOrder = 0;

    template <class TBase, class TGroup = void, int TMinOrder = SystemMinOrder, int TMaxOrder = SystemMaxOrder>
    class SystemBaseT : TBase
    {
    public:
        constexpr static int MinOrder = TMinOrder;
        constexpr static int MaxOrder = TMaxOrder;
        constexpr static int Order = static_cast<int32_t>((static_cast<int64_t>(TMinOrder) + static_cast<int64_t>(TMaxOrder)) / 2);
        using Group = TGroup;

        SystemBaseT(): TBase(std::is_void_v<TGroup> ? std::nullopt : typeid(Group), Order)
        {
        }
    };

    template <class TBase, class TParentSystem, int Order>
    class AbsoluteSystemBaseT : SystemBaseT<TBase, TParentSystem, Order, Order>
    {
    };

    enum class OrderRelation:uint8_t
    {
        Before,
        After
    };

    template <class TBase, class TBrotherSystem, OrderRelation Relation>
    class RelativeSystemBaseT : SystemBaseT<TBase, typename TBrotherSystem::Group,
                                            Relation == OrderRelation::Before ? TBrotherSystem::MinOrder : TBrotherSystem::Order,
                                            Relation == OrderRelation::Before ? TBrotherSystem::Order : TBrotherSystem::MaxOrder>
    {
        static_assert(TBrotherSystem::MaxOrder - TBrotherSystem::MinOrder >= 2 && "相对的系统不支持扩展范围！");
    };
}