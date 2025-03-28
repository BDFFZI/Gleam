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
        static void AddSetting(std::string_view name, const Type& type);

        static bool HasSetting(const std::string_view name)
        {
            if (!settings.contains(name.data()))
                return false;

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
            {
                const Type& type = *settings.at(std::string(name));
                assetBundle.EmplaceAsset(Asset{type.MakeShared(type.Create()), type}, 1);
            }
#ifdef GleamEngineEditor
            std::string filePath = std::string("ProjectSettings/") + name.data();
            AssetDatabase::Create(filePath, assetBundle);
#else
            Resources::Save(assetBundle);
#endif
            AssetBundle::Unload(assetBundle);
        }
        static void LoadSetting(const std::string_view name)
        {
#ifdef GleamEngineEditor
            std::string filePath = std::string("ProjectSettings/") + name.data();
            if (!std::filesystem::exists(filePath))
                return;
            AssetDatabase::Load(filePath);
#else
            Resources::Load(MD5(name.data()).toArray());
#endif
        }
        static void SaveSetting(std::string_view name)
        {
            AssetBundle& assetBundle = AssetBundle::Create(MD5(name.data()).toArray());
            {
                const Type& type = *settings.at(std::string(name));
                assetBundle.EmplaceAsset(Asset{type.MakeShared(type.Create()), type}, 1);
            }
#ifdef GleamEngineEditor
            std::string filePath = std::string("ProjectSettings/") + name.data();
            AssetDatabase::Create(filePath, assetBundle);
#else
            Resources::Save(assetBundle);
#endif
            AssetBundle::Unload(assetBundle);
        }
        static void UnloadSetting(const std::string_view name)
        {
            SaveSetting(name);
        }

    private:
        friend void SettingManager_LoadSettings();
        friend void SettingManager_UnloadSettings();

        inline static std::unordered_map<std::string, const Type*> settings = {};
    };
#define Gleam_AddSetting(name,type) Gleam_MakeInitEvent(){::Gleam::SettingManager::AddSetting(name, type##Type);}

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