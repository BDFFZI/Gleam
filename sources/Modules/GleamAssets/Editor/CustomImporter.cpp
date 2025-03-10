#include "CustomImporter.h"

#include "GleamECS/Runtime/Scene.h"

namespace Gleam
{
    AssetBundle& SceneImporter::ImportAsset(const std::filesystem::path& assetPath, uuids::uuid& assetBundleID)
    {
        AssetBundle& assetBundle = AssetBundle::LoadJson(assetPath.string(), true);
        assetBundleID = assetBundle.GetID();
        Scene::Destroy(assetBundle.GetObject<SceneAsset>(0).name);
        Scene::FromAssetBundle(assetBundle); //加载场景到世界
        return assetBundle;
    }
}