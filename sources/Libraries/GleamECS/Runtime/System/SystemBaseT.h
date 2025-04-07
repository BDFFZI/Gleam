#pragma once
#include <cstdint>
#include <limits>
#include <optional>
#include <typeindex>
#include <type_traits>

namespace Gleam
{
    static constexpr int32_t SystemMinOrder = std::numeric_limits<int32_t>::lowest();
    static constexpr int32_t SystemMaxOrder = std::numeric_limits<int32_t>::max();
    static constexpr int32_t SystemMidOrder = 0;

    template <typename T>
    consteval int32_t GetSystemMinOrder()
    {
        if constexpr (std::is_void_v<T>)
            return SystemMinOrder;
        else
            return T::MinOrder;
    }

    template <typename T>
    consteval int32_t GetSystemMaxOrder()
    {
        if constexpr (std::is_void_v<T>)
            return SystemMaxOrder;
        else
            return T::MaxOrder;
    }

    template <typename T>
    consteval int32_t GetSystemOrder()
    {
        if constexpr (std::is_void_v<T>)
            return SystemMidOrder;
        else
            return T::Order;
    }

    template <class TBase, class TGroup = void, int TMinOrder = SystemMinOrder, int TMaxOrder = SystemMaxOrder>
    class SystemBaseT : public TBase
    {
    public:
        constexpr static int MinOrder = TMinOrder;
        constexpr static int MaxOrder = TMaxOrder;
        constexpr static int Order = static_cast<int32_t>((static_cast<int64_t>(TMinOrder) + static_cast<int64_t>(TMaxOrder)) / 2);
        using Group = TGroup;
    };

    template <class TBase, class TParentSystem, int Order>
    class AbsoluteSystemBaseT : public SystemBaseT<TBase, TParentSystem, Order, Order>
    {
    };

    enum class OrderRelation:uint8_t
    {
        Before,
        After
    };

    template <class TBase, class TBrotherSystem, OrderRelation Relation>
    class RelativeSystemBaseT : public SystemBaseT<TBase, typename TBrotherSystem::Group,
                                                   Relation == OrderRelation::Before ? GetSystemMinOrder<TBrotherSystem>() : GetSystemOrder<TBrotherSystem>(),
                                                   Relation == OrderRelation::Before ? GetSystemOrder<TBrotherSystem>() : GetSystemMaxOrder<TBrotherSystem>()>
    {
        static_assert(static_cast<int64_t>(GetSystemMaxOrder<TBrotherSystem>()) - static_cast<int64_t>(GetSystemMinOrder<TBrotherSystem>()) >= 2 && "相对的系统不支持扩展范围！");
    };
}