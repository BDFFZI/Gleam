#pragma once
#include "GleamReflection/Runtime/Type.h"
#include "GleamUtility/Runtime/Macro.h"

namespace Gleam
{
    class SettingManager
    {
    public:
        static void AddSetting(std::string_view name, const Type& type);
        static void LoadSetting(std::string_view name);
        static void SaveSetting(std::string_view name);

    private:
        friend void SettingManager_LoadSettings();
        friend void SettingManager_SaveSettings();

        inline static std::unordered_map<std::string, const Type*> settings = {};
    };
#define Gleam_AddSetting(name,type) Gleam_MakeInitEvent(){::Gleam::SettingManager::AddSetting(name, type##Type);}

    void SettingManager_LoadSettings();
    void SettingManager_SaveSettings();
}