#include <iostream>

#include "GleamAssets/Editor/EditorSceneManager.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"
#include "GleamAssets/Editor/AssetDatabase/AssetDatabase.h"
#include "GleamAssets/Editor/AssetDatabase/AssetImporter.h"
#include "GleamAssets/Editor/System/ProjectWindow.h"
#include "GleamAssets/Runtime/SceneAssetBundle.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamECS/Runtime/View/View.h"

Gleam_Main

using namespace Gleam;

struct MyComponent
{
    int value;
};
Gleam_MakeType(MyComponent)
{
    Gleam_MakeType_AddField(value);
}

class MySystem : public System<>
{
    void Start() override
    {
        World::GetView<MyComponent>().Each([&](auto& component)
        {
            std::cout << component.value << std::endl;
        });
    }
    void Update() override
    {
        World::GetView<MyComponent>().Each([&](auto& component)
        {
            ++component.value;
        });
    }
    void Stop() override
    {
        World::GetView<MyComponent>().Each([&](auto& component)
        {
            std::cout << component.value << std::endl;
        });
    }
};
Gleam_MakeEngineStartEvent(Init, 0)
{
    {
        Scene& scene = World::AddScene("TestScene");
        World::AddSceneEntity(MyComponent{});
        World::AddSceneSystem<MySystem>();
        {
            AssetBundle& assetBundle = AssetBundle::Create(MD5("TestScene").toArray());
            SceneAssetBundle::MapToAssetBundle(scene, assetBundle);
            std::filesystem::create_directories("./Assets/Scenes");
            AssetDatabase::Create("Assets/Scenes/TestScene.scene", assetBundle);
            AssetBundle::Unload(assetBundle);
        }
        World::RemoveScene(scene);
    }

    EditorSceneManager::OpenScene("Assets/Scenes/TestScene.scene");
}