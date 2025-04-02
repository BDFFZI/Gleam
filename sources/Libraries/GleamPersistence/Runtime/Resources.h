#pragma once
#include <filesystem>

#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    class Resources
    {
    public:
        static bool Has(uuids::uuid assetBundleID);
        static AssetBundle& Load(uuids::uuid assetBundleID);
        static void Reload(uuids::uuid assetBundleID);
        static void Unload(AssetBundle& assetBundle);

        static void Create(AssetBundle& assetBundle);
        static void Save(AssetBundle& assetBundle);

    private:
        inline static std::filesystem::path resourceDirectory = "Library/Artifacts";
        inline static std::unordered_map<uuids::uuid, int> assetBundleRefCount = {};
    };
}