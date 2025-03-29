#pragma once
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamUtility/Runtime/Macro.h"

#ifdef GleamEngineEditor
#include "GleamAssets/Editor/Asset/AssetDatabase.h"
#endif

namespace Gleam
{
    /**
     * 设置是一种针对全局数据的持久化文件，被注册为设置的静态类，将在引擎启动和关闭时自动加载（或创建）、保存数据。
     */
    class SettingManager
    {
    public:
        static void MakeSetting(std::string_view name, const Type& type);

    private:
        friend void SettingManager_LoadSettings();
        friend void SettingManager_UnloadSettings();

        static bool HasSetting(const std::string_view name)
        {
#ifdef GleamEngineEditor
            std::string filePath = std::string("ProjectSettings/") + name.data();
            return std::filesystem::exists(filePath);
#else
            return Resources::Has(MD5(name.data()).toArray());
#endif
        }
        static void CreateSetting(const std::string_view name)
        {
            AssetBundle& assetBundle = AssetBundle::Create(MD5(name.data()).toArray());
            const Type& type = *settings.at(std::string(name));
            assetBundle.AddAsset(Asset{type.MakeShared(type.Create()), type});
#ifdef GleamEngineEditor
            std::string filePath = std::string("ProjectSettings/") + name.data();
            AssetDatabase::Create(filePath, assetBundle);
#else
            Resources::Create(assetBundle);
#endif
            AssetBundle::Unload(assetBundle);
        }
        static void LoadSetting(const std::string_view name)
        {
            if (!HasSetting(name))
                CreateSetting(name);

#ifdef GleamEngineEditor
            std::string filePath = std::string("ProjectSettings/") + name.data();
            AssetDatabase::Load(filePath);
#else
            Resources::Load(MD5(name.data()).toArray());
#endif
        }
        static void SaveSetting(std::string_view name)
        {
#ifdef GleamEngineEditor
            std::string filePath = std::string("ProjectSettings/") + name.data();
            AssetDatabase::Save(filePath);
#else
            Resources::Save(AssetBundle::GetAssetBundle(MD5(name.data()).toArray()));
#endif
        }
        static void UnloadSetting(const std::string_view name)
        {
            SaveSetting(name);

#ifdef GleamEngineEditor
            std::string filePath = std::string("ProjectSettings/") + name.data();
            AssetDatabase::Unload(filePath);
#else
            Resources::Unload(AssetBundle::GetAssetBundle(MD5(name.data()).toArray()));
#endif
        }

        inline static std::unordered_map<std::string, const Type*> settings = {};
    };
#define Gleam_MakeSetting(name,type) Gleam_MakeInitEvent(){::Gleam::SettingManager::MakeSetting(name, type##Type);}

    inline void SettingManager_LoadSettings()
    {
        for (const auto& name : SettingManager::settings | std::views::keys)
            SettingManager::LoadSetting(name);
    }
    inline void SettingManager_UnloadSettings()
    {
        for (const auto& name : SettingManager::settings | std::views::keys)
            SettingManager::UnloadSetting(name);
    }
}