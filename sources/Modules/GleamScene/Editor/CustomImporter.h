#pragma once
#include "Asset/AssetImporter.h"

namespace Gleam
{
    class JsonObjectImporter : public AssetImporter
    {
        void LoadAsset(const std::filesystem::path& path, uuids::uuid& assetBundleID) override;
    };
}