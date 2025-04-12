#pragma once
#include <cstdint>
#include <limits>

#include "GleamECS/Runtime/Entity/EntityAllocator.h"

namespace Gleam
{
    struct ISystemEvent
    {
        ISystemEvent() = default;
        ISystemEvent(const ISystemEvent&) = delete;
        ISystemEvent& operator=(const ISystemEvent&) = delete;
        virtual ~ISystemEvent() = default;

        virtual void Start()
        {
        }
        virtual void Update()
        {
        }
        virtual void Stop()
        {
        }
    };
    struct IOrderedSystemEvent : virtual ISystemEvent
    {
        IOrderedSystemEvent(const int order): order(order)
        {
        }
        int GetOrder() const { return order; }

    private:
        int order;
    };
    struct IOrderedSystemEventComparer
    {
        bool operator()(const IOrderedSystemEvent* left, const IOrderedSystemEvent* right) const
        {
            if (left->GetOrder() == right->GetOrder())
                return left < right;
            return left->GetOrder() < right->GetOrder();
        }
    };

    static constexpr int32_t SystemMinOrder = std::numeric_limits<int32_t>::lowest();
    static constexpr int32_t SystemMaxOrder = std::numeric_limits<int32_t>::max();
    static constexpr int32_t SystemMidOrder = 0;

    enum class SystemRelation:uint8_t
    {
        Before,
        After
    };

    /**
     * 系统是一种高级的封装版事件。
     * 通过相对位置、嵌套等结构，实现在引擎中自由轻松的嵌入各种代码。
     *
     * 系统的设计参考了现实中的自然法则：法则没有开始和结束的说法，法则总是存在，只是每条法则作用的环境不一样。
     * 因此作为等价于法则的系统，便是静态类且只有Update方法，并始终通过View来获取目标。
     */
    template <class TGroup = void, int TMinOrder = SystemMinOrder, int TMaxOrder = SystemMaxOrder>
    struct ISystem : IOrderedSystemEvent
    {
        constexpr static int MinOrder = TMinOrder;
        constexpr static int MaxOrder = TMaxOrder;
        constexpr static int Order = static_cast<int32_t>((static_cast<int64_t>(TMinOrder) + static_cast<int64_t>(TMaxOrder)) / 2);
        using Group = TGroup;

        ISystem(): IOrderedSystemEvent(Order)
        {
        }
    };
    template <class TParentSystem, int Order>
    struct IAbsoluteSystem : ISystem<TParentSystem, Order, Order>
    {
    };
    template <class TBrotherSystem, SystemRelation Relation>
        requires (static_cast<int64_t>(TBrotherSystem::MaxOrder) - static_cast<int64_t>(TBrotherSystem::MinOrder) >= 2)
    struct IRelativeSystem : ISystem<typename TBrotherSystem::Group,
                                     Relation == SystemRelation::Before ? TBrotherSystem::MinOrder : TBrotherSystem::Order,
                                     Relation == SystemRelation::Before ? TBrotherSystem::Order : TBrotherSystem::MaxOrder>
    {
    };


    template <class TGroup = void, auto... VArgs>
    class System : public ISystem<TGroup, SystemMinOrder, SystemMaxOrder>
    {
    };

    template <class TGroup, int TMinOrder, int TMaxOrder>
    class System<TGroup, TMinOrder, TMaxOrder> : public ISystem<TGroup, TMinOrder, TMaxOrder>
    {
    };

    template <class TBrotherSystem, SystemRelation Relation>
        requires (static_cast<int64_t>(TBrotherSystem::MaxOrder) - static_cast<int64_t>(TBrotherSystem::MinOrder) >= 2)
    class System<TBrotherSystem, Relation> : public ISystem<typename TBrotherSystem::Group,
                                                            Relation == SystemRelation::Before ? TBrotherSystem::MinOrder : TBrotherSystem::Order,
                                                            Relation == SystemRelation::Before ? TBrotherSystem::Order : TBrotherSystem::MaxOrder>
    {
    };
}