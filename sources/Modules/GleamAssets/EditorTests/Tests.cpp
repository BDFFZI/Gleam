#include <iostream>

#include "GleamAssets/Editor/EditorSceneManager.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"
#include "GleamAssets/Editor/Asset/AssetDatabase.h"
#include "GleamAssets/Editor/Asset/AssetImporter.h"
#include "GleamAssets/Editor/System/ProjectWindow.h"
#include "GleamAssets/Runtime/Asset/BasicSceneInfo.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamECS/Runtime/View.h"

Gleam_Main

using namespace Gleam;

struct MyAsset
{
    BasicSceneInfo* sceneAsset;
    int value;
};
Gleam_MakeTypeWithID(MyAsset, "")
{
    Gleam_MakeType_AddField(sceneAsset);
    Gleam_MakeType_AddField(value);
}
Gleam_MakeCreateAssetMenu(MyAsset, ".myAsset")

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
    void Update() override
    {
        View<MyComponent>::Each([&](auto& component)
        {
            ++component.value;
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
            SceneAssetBundle::SaveToAssetBundle(scene, assetBundle);
            std::filesystem::create_directories("./Assets/Scenes");
            AssetDatabase::Create("Assets/Scenes/TestScene.scene", assetBundle);
            AssetBundle::Unload(assetBundle);
        }
        Scene::Destroy(scene);
    }

    EditorSceneManager::OpenScene("Assets/Scenes/TestScene.scene");
}