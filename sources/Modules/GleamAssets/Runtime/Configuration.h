#pragma once
#include "GleamPersistence/Runtime/JsonUtility.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamUtility/Runtime/File.h"
#include "GleamUtility/Runtime/Macro.h"

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
        static void MakeConfiguration(std::string_view path, const Type& type, bool isAutoSave = false)
        {
            configurations.emplace(path, std::make_tuple(&type, isAutoSave));
        }

        static bool Has(const std::filesystem::path& path)
        {
            return exists(path);
        }
        static void Load(const std::filesystem::path& path)
        {
            const Type& type = *std::get<0>(configurations.at(path));
            void* instance = type.Create();
            JsonUtility::FromJson(File::ReadAllText(path), type, instance);
            type.Destroy(instance);
        }
        static void Save(const std::filesystem::path& path)
        {
            const Type& type = *std::get<0>(configurations.at(path));
            void* instance = type.Create();
            File::WriteAllText(path, JsonUtility::ToJson(instance, type, true));
            type.Destroy(instance);
        }

    private:
        friend void Configuration_LoadSettings();
        friend void Configuration_UnloadSettings();

        inline static std::unordered_map<std::filesystem::path, std::tuple<const Type*, bool>> configurations = {};
    };
#define Gleam_MakeConfiguration(path,type) Gleam_MakeInitEvent(){::Gleam::Configuration::MakeConfiguration(path,Type::CreateOrGet<type>());}
#define Gleam_MakeConfigurationWithAutoSave(path,type) Gleam_MakeInitEvent(){::Gleam::Configuration::MakeConfiguration(path,Type::CreateOrGet<type>(),true);}

    inline void Configuration_LoadSettings()
    {
        for (const auto& path : Configuration::configurations | std::views::keys)
            if (Configuration::Has(path)) Configuration::Load(path);
            else Configuration::Save(path); //新建
    }
    inline void Configuration_UnloadSettings()
    {
        for (const auto& [path,properties] : Configuration::configurations)
            if (std::get<1>(properties) == true) Configuration::Save(path);
    }
}