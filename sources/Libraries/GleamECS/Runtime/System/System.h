#pragma once
#include "SystemInfoAllocator.h"
#include "GleamECS/Runtime/Entity/EntityAllocator.h"
#include "GleamECS/Runtime/View/View.h"

namespace Gleam
{
    class World;
    class System;


    /**
     * 系统是一种高级的封装版事件。
     * 通过相对位置、嵌套等结构，实现在引擎中自由轻松的嵌入各种代码。
     */
    // ReSharper disable once CppClassNeedsConstructorBecauseOfUninitializedMember
    class System
    {
    public:
        virtual ~System() = default;

        System& GetGroup() const
        {
            return *group;
        }
        int GetOrder() const
        {
            return order;
        }

    protected:
        World& GetWorld() const
        {
            return *world;
        }
        EntityAllocator& GetEntities() const;
        template <class... T>
        View<T...> GetView()
        {
            return View<T...>(GetEntities());
        }

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
        friend class SystemAllocator;
        friend class SystemGroup;

        //由World创建时赋值
        World* world = nullptr;
        System* group = nullptr;
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