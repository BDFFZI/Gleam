#pragma once

#include <filesystem>
#include <unordered_map>

#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    class AssetMeta
    {
    public:
        static void AddCustomMeta(std::string_view extension, Type& type);
        static bool HasMeta(const std::filesystem::path& assetPath);
        static std::unique_ptr<AssetMeta> GetMeta(const std::filesystem::path& assetPath);

        virtual ~AssetMeta() = default;

        uuids::uuid GetAssetBundleID() const { return assetBundleID; }
        const std::filesystem::path& GetAssetPath() const { return assetPath; }

        void SaveAndReloadAsset();

        virtual void LoadAsset(uuids::uuid& assetBundleID)
        {
            throw std::runtime_error("导入功能未实现！");
        }
        virtual void UnLoadAsset();
        virtual void SaveAsset();

    private:
        Gleam_MakeType_Friend;
        inline static std::unordered_map<std::string, const Type*> customMetas = {};

        uuids::uuid assetBundleID;
        std::filesystem::path assetPath;
    };

    Gleam_MakeType(AssetMeta, "D59FAA1C-781F-4917-B2E3-AD07A38A9467")
    {
        Gleam_MakeType_AddField(assetBundleID);
    }

#define Gleam_MakeImporter(extension,type) \
    Gleam_MakeInitEvent(){AssetMeta::AddCustomMeta(extension,type);}
}