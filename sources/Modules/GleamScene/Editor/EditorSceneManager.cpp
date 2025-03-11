#include "EditorSceneManager.h"

#include "Asset/AssetDatabase.h"

namespace Gleam
{
    Scene& EditorSceneManager::OpenScene(const std::filesystem::path& path)
    {
        auto it = std::ranges::find_if(sceneAssets, [&path](auto& pair) { return pair.second == path; });
        if (it != sceneAssets.end())
            CloseScene(*it->first);

        AssetBundle& assetBundle = AssetDatabase::Load(path);
        Scene& scene = Scene::FromAssetBundle(assetBundle);
        AssetDatabase::Unload(path);

        sceneAssets.emplace(&scene, path);
        return scene;
    }
    void EditorSceneManager::CloseScene(Scene& scene)
    {
        sceneAssets.erase(&scene);
        Scene::Destroy(scene);
    }
    void EditorSceneManager::SaveScene(Scene& scene)
    {
        auto& path = sceneAssets.at(&scene);

        AssetBundle& assetBundle = AssetDatabase::Load(path);
        Scene::ToAssetBundle(scene, assetBundle);
        AssetDatabase::Save(path);
        AssetDatabase::Unload(path);
    }
}