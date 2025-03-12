#pragma once

#include <filesystem>
#include <unordered_map>

#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    class AssetImporter
    {
    public:
        static void AddCustomImporter(std::string_view extension, Type& type);
        static bool CanImport(const std::filesystem::path& assetPath);
        static AssetImporter& GetImporter(const std::filesystem::path& assetPath);

        virtual ~AssetImporter() = default;

        uuids::uuid GetAssetBundleID() const { return assetBundleID; }
        const std::filesystem::path& GetAssetPath() const { return assetPath; }

        uuids::uuid SaveAndReloadAsset();

    protected:
        virtual void LoadAsset(const std::filesystem::path& path, uuids::uuid& assetBundleID)
        {
            throw std::runtime_error("导入功能未实现！");
        }

    private:
        Gleam_MakeType_Friend;
        inline static std::unordered_map<std::string, const Type*> customImporters = {};
        inline static std::unordered_map<std::filesystem::path, std::unique_ptr<AssetImporter>> cacheImporters = {};

        uuids::uuid assetBundleID;
        std::filesystem::path assetPath;
    };

    Gleam_MakeType(AssetImporter, "D59FAA1C-781F-4917-B2E3-AD07A38A9467")
    {
        Gleam_MakeType_AddField(assetBundleID);
    }

#define Gleam_MakeAssetImporter(extension,type) \
    Gleam_MakeInitEvent(){AssetImporter::AddCustomImporter(extension,type);}
}