#include "CustomMeta.h"

#include "GleamECS/Runtime/Scene.h"

namespace Gleam
{
    void SceneImporter::LoadAsset(uuids::uuid& assetBundleID)
    {
        AssetBundle& assetBundle = AssetBundle::LoadJson(GetAssetPath().string(), true);
        Scene::Destroy(assetBundle.GetObject<SceneAsset>(0).name); //卸载旧场景
        Scene::FromAssetBundle(assetBundle); //加载场景到世界
        assetBundleID = assetBundle.GetID(); //矫正资源包ID
    }
    void SceneImporter::SaveAsset()
    {
        AssetBundle& assetBundle = AssetBundle::GetAssetBundle(GetAssetBundleID());
        //填充资源包信息
        std::string sceneName = assetBundle.GetObject<SceneAsset>(0).name;
        auto optionalScene = Scene::GetScene(sceneName);
        if (optionalScene.has_value())
            Scene::ToAssetBundle(optionalScene.value(), assetBundle);
        //保存
        AssetBundle::SaveJson(GetAssetPath().string(), assetBundle);
    }
}