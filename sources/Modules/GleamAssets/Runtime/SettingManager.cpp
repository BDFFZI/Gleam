#include "SettingManager.h"

#include "GleamAssets/Editor/Asset/AssetDatabase.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    void SettingManager::AddSetting(std::string_view name, const Type& type)
    {
        settings.emplace(name, &type);
    }
    void SettingManager_LoadSettings()
    {
        for (const auto& name : SettingManager::settings | std::views::keys)
            SettingManager::LoadSetting(name);
    }
    void SettingManager_SaveSettings()
    {
        for (const auto& name : SettingManager::settings | std::views::keys)
            SettingManager::SaveSetting(name);
    }
}