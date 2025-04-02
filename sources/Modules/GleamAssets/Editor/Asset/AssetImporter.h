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
        static void MakeAssetImporter(std::string_view extension, Type& type);
        static void MakeAssetImporter(std::string_view extension, const Type& type);
        static bool CanImport(const std::filesystem::path& assetPath);
        static AssetImporter& GetImporter(const std::filesystem::path& assetPath, bool reload = false);

        virtual ~AssetImporter() = default;

        const std::filesystem::path& GetAssetPath() const { return assetPath; }
        uuids::uuid GetAssetBundleID() const { return assetBundleID; }

        void Save();
        AssetBundle SaveAndReadAsset();

    protected:
        virtual AssetBundle ReadAsset(const std::filesystem::path& path, uuids::uuid lastAssetBundleID)
        {
            throw std::runtime_error("导入功能未实现！");
        }

    private:
        Gleam_MakeType_Friend;
        friend class AssetDatabase;

        inline static std::unordered_map<std::string, const Type*> customImporters = {};
        inline static std::unordered_map<std::filesystem::path, std::unique_ptr<AssetImporter>> cacheImporters = {};

        std::filesystem::path assetPath;
        uuids::uuid assetBundleID;
        int64_t assetTimeStamp;
        uuids::uuid assetContentHash;
    };

    Gleam_MakeTypeWithID(AssetImporter, "D59FAA1C-781F-4917-B2E3-AD07A38A9467")
    {
        Gleam_MakeType_AddField(assetBundleID);
        Gleam_MakeType_AddField(assetTimeStamp);
        Gleam_MakeType_AddField(assetContentHash);
    }

    class JsonObjectImporter : public AssetImporter
    {
        AssetBundle ReadAsset(const std::filesystem::path& path, uuids::uuid lastAssetBundleID) override
        {
            return AssetBundle::ReadJson(path);
        }
    };
    Gleam_MakeTypeWithIDParent(JsonObjectImporter, "", AssetImporterType)
    {
    }

#define Gleam_MakeAssetImporter(extension,type) \
    Gleam_MakeInitEvent(){::Gleam::AssetImporter::MakeAssetImporter(extension,type);}
}