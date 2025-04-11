#pragma once
#include "System.h"
#include "GleamReflection/Runtime/Type.h"
#include <variant>

namespace Gleam
{
    struct SystemInfo
    {
        const Type* type;
        SystemInfo* group;
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
        static SystemInfo& CreateOrGetSystemInfo()
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

            return systemInfo;
        }
        static SystemInfo& GetSystemInfo(const uuids::uuid typeID)
        {
            return systemInfoMap.at(typeID);
        }

    private:
        inline static std::unordered_map<uuids::uuid, SystemInfo> systemInfoMap;
    };
}