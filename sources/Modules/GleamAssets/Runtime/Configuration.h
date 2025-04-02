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
     * 配置是一种针对全局数据的持久化文件
     *
     * 被注册为配置的静态类，将在引擎启动时自动加载或创建序列化文件。此外根据设置，还可以在引擎停止时自动保存。
     */
    class Configuration
    {
    public:
        static void MakeConfiguration(std::string_view name, const Type& type);

    private:
        friend void Configuration_LoadSettings();
        friend void Configuration_UnloadSettings();

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
            // SaveSetting(name); //不支持自动保存

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

    inline void Configuration_LoadSettings()
    {
        for (const auto& name : Configuration::settings | std::views::keys)
            Configuration::LoadSetting(name);
    }
    inline void Configuration_UnloadSettings()
    {
        for (const auto& name : Configuration::settings | std::views::keys)
            Configuration::UnloadSetting(name);
    }
}