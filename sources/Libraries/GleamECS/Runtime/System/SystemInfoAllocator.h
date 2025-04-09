#pragma once
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct SystemInfo
    {
        const Type* type;
        SystemInfo* group;
        int order;
    };

    class SystemInfoAllocator
    {
    public:
        static auto GetAllSystemInfo()
        {
            return systemInfoMap | std::views::values | std::views::transform([](SystemInfo& systemInfo) { return std::reference_wrapper(systemInfo); });
        }
        template <class TSystem> requires requires() { typename TSystem::Group;TSystem::Order; }
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

    private:
        inline static std::unordered_map<uuids::uuid, SystemInfo> systemInfoMap;
    };

#define Gleam_MakeSystem(systemClass) \
inline ::Gleam::SystemInfo& systemClass##Info = ::Gleam::SystemInfoAllocator::CreateOrGetSystemInfo<systemClass>();

    static constexpr int32_t SystemMinOrder = std::numeric_limits<int32_t>::lowest();
    static constexpr int32_t SystemMaxOrder = std::numeric_limits<int32_t>::max();
    static constexpr int32_t SystemMidOrder = 0;

    enum class OrderRelation:uint8_t
    {
        Before,
        After
    };

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

        SystemBaseT(): TBase(Order)
        {
        }
    };

    template <class TBase, class TParentSystem, int Order>
    class AbsoluteSystemBaseT : public SystemBaseT<TBase, TParentSystem, Order, Order>
    {
    };


    template <class TBase, class TBrotherSystem, OrderRelation Relation>
    class RelativeSystemBaseT : public SystemBaseT<TBase, typename TBrotherSystem::Group,
                                                   Relation == OrderRelation::Before ? GetSystemMinOrder<TBrotherSystem>() : GetSystemOrder<TBrotherSystem>(),
                                                   Relation == OrderRelation::Before ? GetSystemOrder<TBrotherSystem>() : GetSystemMaxOrder<TBrotherSystem>()>
    {
        static_assert(static_cast<int64_t>(GetSystemMaxOrder<TBrotherSystem>()) - static_cast<int64_t>(GetSystemMinOrder<TBrotherSystem>()) >= 2 && "相对的系统不支持扩展范围！");
    };
}