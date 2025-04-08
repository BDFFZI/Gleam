#pragma once
#include "SystemInfoAllocator.h"

namespace Gleam
{
    /**
     * 系统是一种高级的封装版事件。
     * 通过相对位置、嵌套等结构，实现在引擎中自由轻松的嵌入各种代码。
     */
    // ReSharper disable once CppClassNeedsConstructorBecauseOfUninitializedMember
    class System
    {
    public:
        System(const int order)
            : order(order)
        {
        }
        virtual ~System() = default;

        int GetOrder() const
        {
            return order;
        }

    protected:
        virtual void Start()
        {
        }
        virtual void Update()
        {
        }
        virtual void Stop()
        {
        }

    private:
        friend class SystemGroup;
        int order = 0;
    };


    template <class TGroup = void, int TMinOrder = SystemMinOrder, int TMaxOrder = SystemMaxOrder>
    class SystemT : public SystemBaseT<System, TGroup, TMinOrder, TMaxOrder>
    {
    };

    template <class TParentSystem, int Order>
    class AbsoluteSystemT : public AbsoluteSystemBaseT<System, TParentSystem, Order>
    {
    };

    template <class TBrotherSystem, OrderRelation Relation>
    class RelativeSystemT : public RelativeSystemBaseT<System, TBrotherSystem, Relation>
    {
    };
}