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
        static void Unload(uuids::uuid assetBundleID);
        
        static void Save(AssetBundle& assetBundle);
        static void Reload(uuids::uuid assetBundleID);

    private:
        inline static std::filesystem::path resourceDirectory = "Library";
        inline static std::unordered_map<uuids::uuid, int> assetBundleRefCount = {};
    };
}