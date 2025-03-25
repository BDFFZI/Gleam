#include "SettingManager.h"

#include "GleamAssets/Editor/Asset/AssetDatabase.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    void SettingManager::AddSetting(std::string_view name, const Type& type)
    {
        settings.emplace(name, &type);
    }

    void SettingManager::LoadSetting(const std::string_view name)
    {
        std::string filePath = std::string("ProjectSettings/") + name.data();
        if (!std::filesystem::exists(filePath))
            return;

        AssetDatabase::Load(filePath);
        AssetDatabase::Unload(filePath); //加载到静态类，因此可以直接卸载
    }
    void SettingManager::SaveSetting(const std::string_view name)
    {
        std::string filePath = std::string("ProjectSettings/") + name.data();
        uuids::uuid id = AssetDatabase::GetAssetBundleID(filePath);

        AssetBundle& assetBundle = AssetBundle::Create(id);
        {
            const Type& type = *settings.at(std::string(name));
            assetBundle.AddAsset(Asset{type.MakeShared(type.Create()), type});
        }
        AssetDatabase::Create(filePath, assetBundle);
        AssetBundle::Unload(assetBundle);
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