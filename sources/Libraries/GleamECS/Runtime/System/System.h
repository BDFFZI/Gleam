#pragma once
#include "SystemBaseT.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    class World;

    /**
     * 系统是一种高级的封装版事件。
     * 通过相对位置、嵌套等结构，实现在引擎中自由轻松的嵌入各种代码。
     */
    class System
    {
    public:
        System(const std::optional<std::type_index> group, const int order) : group(group), order(order)
        {
        }
        System(const System&) = delete;
        System& operator=(const System&) = delete;
        System(System&&) = delete;
        System& operator=(System&&) = delete;
        virtual ~System() = default;

        World& GetWorld() const
        {
            return *world;
        }
        std::optional<std::type_index> GetGroup() const
        {
            return group;
        }
        int GetOrder() const
        {
            return order;
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
        friend World;

        World* world = nullptr; //由World修改
        std::optional<std::type_index> group;
        int order;
    };


    template <class TGroup = void, int TMinOrder = SystemMinOrder, int TMaxOrder = SystemMaxOrder>
    class SystemT : SystemBaseT<System, TGroup, TMinOrder, TMaxOrder>
    {
    };

    template <class TParentSystem, int Order>
    class AbsoluteSystemT : AbsoluteSystemBaseT<System, TParentSystem, Order>
    {
    };

    template <class TBrotherSystem, OrderRelation Relation>
    class RelativeSystemT : RelativeSystemBaseT<System, TBrotherSystem, Relation>
    {
    };

#define Gleam_MakeGlobalSystem(systemClass) \
inline systemClass& Global##systemClass = ::Gleam::System::CreateGlobal<systemClass>("",::Gleam::Type::CreateOrGet<systemClass>().GetID());
#define Gleam_MakeGlobalSystemWithID(systemClass,uuidStr) \
inline systemClass& Global##systemClass = ::Gleam::System::CreateGlobal<systemClass>("",uuids::uuid::from_string(uuidStr).value());
}