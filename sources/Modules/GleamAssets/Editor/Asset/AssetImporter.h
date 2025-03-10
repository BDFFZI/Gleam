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
        static void AddCustomImporter(std::string_view extension, const Type& type);
        static bool HasImporter(const std::filesystem::path& assetPath);
        static AssetImporter& GetImporter(const std::string& assetPath);

        AssetBundle& ImportAsset();
        void SaveAndReimport();
    private:
        inline static std::unordered_map<std::string, const Type*> customImporters = {};

        std::filesystem::path path;
    };
}