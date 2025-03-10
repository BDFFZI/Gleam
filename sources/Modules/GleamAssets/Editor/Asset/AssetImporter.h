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

        static bool HasImporter(const std::filesystem::path& assetPath);
        static AssetBundle& LoadImporter(const std::filesystem::path& assetPath);
        static void SaveImporter(AssetBundle& importer);
        static void UnLoadImporter(AssetBundle& importer);

        virtual ~AssetImporter() = default;

        AssetBundle& Import();

    protected:
        virtual AssetBundle& ImportAsset(const std::filesystem::path& assetPath, uuids::uuid& assetBundleID)
        {
            throw std::runtime_error("导入器未实现！");
        }

    private:
        Gleam_MakeType_Friend
        friend class AssetDatabase;
        inline static std::unordered_map<std::string, const Type*> customImporters = {};

        uuids::uuid assetBundleID;
        std::filesystem::path assetPath;
    };

    Gleam_MakeType(AssetImporter, "D59FAA1C-781F-4917-B2E3-AD07A38A9467")
    {
        Gleam_MakeType_AddField(assetBundleID);
    }

#define Gleam_MakeImporter(extension,type) \
    Gleam_MakeInitEvent(){AssetImporter::AddCustomImporter(extension,type);}
}