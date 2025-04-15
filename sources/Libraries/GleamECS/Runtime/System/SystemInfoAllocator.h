#pragma once
#include "System.h"
#include "SystemGroup.h"
#include "GleamReflection/Runtime/Type.h"
#include <variant>

namespace Gleam
{
    struct SystemInfo
    {
        const Type* type;
        const SystemInfo* group;
        bool isGroup;
    };

    /**
     * 用于支持运行时（非模板）增删系统
     */
    class SystemInfoAllocator
    {
    public:
        static auto GetAllSystemInfo()
        {
            return systemInfoMap | std::views::values | std::views::transform([](SystemInfo& systemInfo) { return std::reference_wrapper(systemInfo); });
        }
        template <class TSystem> requires requires() { typename TSystem::Group;TSystem::Order; }
        static const SystemInfo& CreateOrGetSystemInfo()
        {
            Type& type = Type::CreateOrGet<TSystem>();
            if (systemInfoMap.contains(type.GetID()))
                return systemInfoMap[type.GetID()];

            SystemInfo& systemInfo = systemInfoMap[type.GetID()];
            systemInfo.type = &type;
            if constexpr (std::is_void_v<typename TSystem::Group>)
                systemInfo.group = nullptr;
            else
                systemInfo.group = &CreateOrGetSystemInfo<typename TSystem::Group>();
            systemInfo.isGroup = std::derived_from<TSystem, ISystemGroup>;

            return systemInfo;
        }
        static const SystemInfo& GetSystemInfo(const uuids::uuid typeID)
        {
            return systemInfoMap.at(typeID);
        }
        static const SystemInfo& GetSystemInfo(const ISystemEvent& system)
        {
            return GetSystemInfo(Type::GetType(typeid(system)).value().get().GetID());
        }
        static bool HasSystemInfo(const uuids::uuid typeID)
        {
            return systemInfoMap.contains(typeID);
        }

    private:
        inline static std::unordered_map<uuids::uuid, SystemInfo> systemInfoMap;
    };
}