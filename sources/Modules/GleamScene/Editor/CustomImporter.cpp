#include "CustomImporter.h"

#include "GleamECS/Runtime/Scene.h"

namespace Gleam
{
    void JsonObjectImporter::LoadAsset(const std::filesystem::path& path, uuids::uuid& assetBundleID)
    {
        AssetBundle& assetBundle = AssetBundle::LoadJson(path.string(), true);
        assetBundleID = assetBundle.GetID();
    }
}