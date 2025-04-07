#pragma once
#include "SystemBaseT.h"
#include "GleamECS/Runtime/Entity/EntityAllocator.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    class World;
    class System;

    struct SystemInfo
    {
        const Type* type;
        SystemInfo* group;
        int order;
    };

    /**
     * 系统是一种高级的封装版事件。
     * 通过相对位置、嵌套等结构，实现在引擎中自由轻松的嵌入各种代码。
     */
    // ReSharper disable once CppClassNeedsConstructorBecauseOfUninitializedMember
    class System
    {
    public:
        template <class TSystem> requires std::derived_from<TSystem, System>
        static SystemInfo& CreateOrGetSystemInfo()
        {
            Type& type = Type::CreateOrGet<TSystem>();
            if (systemInfoMap.contains(type.GetID()))
                return systemInfoMap[type.GetID()];

            SystemInfo& systemInfo = systemInfoMap[type.GetID()];
            if constexpr (std::is_void_v<typename TSystem::Group>)
                systemInfo.group = nullptr;
            else
                systemInfo.group = &CreateOrGetSystemInfo<typename TSystem::Group>();
            systemInfo.order = TSystem::Order;
            systemInfo.type = &type;
            return systemInfo;
        }
        static SystemInfo& GetSystemInfo(const uuids::uuid typeID)
        {
            return systemInfoMap.at(typeID);
        }

        virtual ~System() = default;

        World& GetWorld() const
        {
            return *world;
        }
        System& GetGroup() const
        {
            return *group;
        }
        int GetOrder() const
        {
            return order;
        }

        EntityAllocator& GetAllocator() const;
        template <class... T>
        View<T...> GetView()
        {
            return View<T...>(GetAllocator());
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
        friend class World;

        inline static std::unordered_map<uuids::uuid, SystemInfo> systemInfoMap;

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

#define Gleam_MakeGlobalSystem(systemClass) \
inline systemClass& Global##systemClass = ::Gleam::System::CreateGlobal<systemClass>("",::Gleam::Type::CreateOrGet<systemClass>().GetID());
#define Gleam_MakeGlobalSystemWithID(systemClass,uuidStr) \
inline systemClass& Global##systemClass = ::Gleam::System::CreateGlobal<systemClass>("",uuids::uuid::from_string(uuidStr).value());
}