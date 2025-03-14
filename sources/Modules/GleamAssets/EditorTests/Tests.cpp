#include <iostream>

#include "GleamAssets/Editor/EditorSceneManager.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"
#include "GleamAssets/Editor/Asset/AssetDatabase.h"
#include "GleamAssets/Editor/Asset/AssetImporter.h"
#include "GleamAssets/Editor/System/ProjectWindow.h"
#include "GleamAssets/Runtime/SceneAsset/SceneAsset.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamECS/Runtime/View.h"

Gleam_Main

using namespace Gleam;

void ProjectWindow_CreateTestAsset()
{
    auto path = ProjectWindow::GetDirectoryDrawing() / "TestAsset.asset";

    AssetBundle& assetBundle = AssetBundle::Create();
    {
        assetBundle.AddAsset(Sphere{{1, 2, 3}, 4});
        AssetDatabase::Save(path, assetBundle);
    }
    AssetBundle::Unload(assetBundle);
}

Gleam_AddProjectWindowDirectoryMenu("Create/TestAsset", ProjectWindow_CreateTestAsset)

struct MyComponent
{
    int value;
};
class MySystem : public System
{
    void Start() override
    {
        View<MyComponent>::Each([&](auto& component)
        {
            std::cout << component.value << std::endl;
        });
    }
    void Stop() override
    {
        View<MyComponent>::Each([&](auto& component)
        {
            std::cout << component.value << std::endl;
        });
    }
};
Gleam_MakeGlobalSystem(MySystem)

Gleam_MakeEngineStartEvent(Init, 0)
{
    {
        Scene& scene = Scene::Create("TestScene");
        scene.AddEntity(World::AddEntity(MyComponent{123}));
        scene.AddSystem(GlobalMySystem);
        {
            AssetBundle& assetBundle = AssetBundle::Create(MD5("TestScene").toArray());
            SceneAsset::ToAssetBundle(scene, assetBundle);
            std::filesystem::create_directories("./Assets/Scenes");
            AssetDatabase::Save("Assets/Scenes/TestScene.scene", assetBundle);
            AssetBundle::Unload(assetBundle);
        }
        Scene::Destroy(scene);
    }

    EditorSceneManager::OpenScene("Assets/Scenes/TestScene.scene");
}