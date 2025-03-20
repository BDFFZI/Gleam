#include "GleamECS/RuntimeTests/Tests.h"

#include <gtest/gtest.h>
#include <stduuid/uuid.h>

#include "GleamAssets/Runtime/SceneManager.h"
#include "GleamAssets/Runtime/SceneAsset/EntityAsset.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamAssets/Runtime/SceneAsset/SceneAsset.h"
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

using namespace Gleam;

struct MyComponent
{
    int value;
    Entity dependency = Entity::Null;
};

Gleam_MakeType(MyComponent, "")
{
    Gleam_MakeType_AddField(value);
    Gleam_MakeType_AddField(dependency);
}

class MySystem : public System
{
    void Update() override
    {
        View<MyComponent>::Each([](MyComponent& myComponent)
        {
            myComponent.value++;
        });
    }
};

// TEST(Assets, Scene)
void main()
{
    MySystem& mySystem1 = System::CreateGlobal<MySystem>("MySystem1");
    MySystem& mySystem2 = System::CreateGlobal<MySystem>("MySystem2");

    //测试场景的创建和保存
    {
        Scene& scene = Scene::Create("TestScene");
        //添加实体
        Entity entity = World::AddEntity(Transform{999}, RigidBody{}, SpringPhysics{});
        scene.AddEntity(entity);
        scene.AddEntity(World::AddEntity(Transform{2}, RigidBody{}));
        scene.AddEntity(World::AddEntity(MyComponent{3, entity}));
        //添加系统
        scene.AddSystem(mySystem1);
        World::Update(); //应用世界更改
        //持久化
        AssetBundle& assetBundle = AssetBundle::Create();
        SceneAsset::ToAssetBundle(scene, assetBundle);
        AssetBundle::SaveJson("TestScene.json", assetBundle);
        AssetBundle::Unload(assetBundle); //卸载资源包不影响，场景内实体
        ASSERT_EQ(View<Transform>::Count(), 2);
        //销毁场景会移除实体
        Scene::Destroy(scene);
        World::Update(); //应用世界更改
        ASSERT_EQ(View<Transform>::Count(), 0);
    }

    //测试加载实体场景
    {
        //加载资源包就会加载实体
        AssetBundle& assetBundle = AssetBundle::LoadJson("TestScene.json");
        World::Update(); //应用世界更改
        ASSERT_EQ(View<Transform>::Count(), 2);
        //场景通过读取资源包恢复数据
        Scene& scene = SceneAsset::FromAssetBundle(assetBundle);
        AssetBundle::Unload(assetBundle); //从资源包内恢复场景后资源包就可以直接删除了（如果不需要再次存储的话）。
        ASSERT_EQ(scene.GetEntities().size(), 3);
        ASSERT_TRUE(scene.HasSystem(mySystem1));
        //验证场景内实体信息正确性
        std::vector<Entity> entities;
        View<MyComponent>::Fetch(entities);
        ASSERT_TRUE(scene.HasEntity(entities[0]));
        MyComponent& myComponent = World::GetComponent<MyComponent>(entities[0]);
        ASSERT_EQ(myComponent.value, 3);
        ASSERT_EQ(World::GetComponent<Transform>(myComponent.dependency).position, 999);

        Scene::Destroy(scene);
        World::Update(); //应用世界更改
    }

    //测试运行场景
    {
        //中途添加系统
        AssetBundle& assetBundle = AssetBundle::LoadJson("TestScene.json");
        Scene& scene = SceneAsset::FromAssetBundle(assetBundle);
        scene.AddSystem(mySystem2);
        ASSERT_EQ(scene.GetSystems().size(), 2);
        //启动场景
        ASSERT_EQ(World::GetRootSystemGroup().GetSubSystems().size(), 0);
        scene.Start();
        World::Update();
        ASSERT_EQ(World::GetRootSystemGroup().GetSubSystems().size(), 2);
        //停止场景
        scene.Stop();
        World::Update();
        ASSERT_EQ(World::GetRootSystemGroup().GetSubSystems().size(), 0);
        //实体被更新
        ASSERT_EQ(World::GetComponent<MyComponent>(assetBundle.GetObject<EntityAsset>(3).GetEntity()).value, 5);
        //写回资源包并卸载场景
        SceneAsset::ToAssetBundle(scene, assetBundle);
        AssetBundle::SaveJson("TestScene.json", assetBundle);
        AssetBundle::Unload(assetBundle);
        Scene::Destroy(scene);
        World::Update(); //应用世界更改
    }

    {
        //重新加载
        AssetBundle& assetBundle = AssetBundle::LoadJson("TestScene.json");
        Scene& scene = SceneAsset::FromAssetBundle(assetBundle);
        AssetBundle::Unload(assetBundle);

        ASSERT_EQ(scene.GetEntities().size(), 3);
        ASSERT_EQ(scene.GetSystems().size(), 2);
        Scene::Destroy(scene);
    }

    World::Clear();
}

class MySystem2 : public System
{
    void Start() override
    {
        View<MyComponent>::Each([](MyComponent& myComponent)
        {
            std::cout << myComponent.value << std::endl;
        });
    }
    void Update() override
    {
        Engine::Stop();
    }
    void Stop() override
    {
        View<MyComponent>::Each([](MyComponent& myComponent)
        {
            std::cout << myComponent.value << std::endl;
        });
    }
};
Gleam_MakeGlobalSystem(MySystem)
Gleam_MakeGlobalSystem(MySystem2)

TEST(Assets, Runtime)
{
    uuids::uuid id = MD5("TestScene2").toArray();
    //持久化一个场景
    {
        Scene& scene = Scene::Create("TestScene2");
        scene.AddEntity(World::AddEntity(MyComponent{123}));
        scene.AddSystem(GlobalMySystem);
        scene.AddSystem(GlobalMySystem2);
        AssetBundle& assetBundle = AssetBundle::Create(id);
        SceneAsset::ToAssetBundle(scene, assetBundle);
        Resources::Save(assetBundle);
        assetBundle.Unload(assetBundle);
        Scene::Destroy(scene);
    }

    //加载场景
    SceneManager::LoadScene(id);

    Engine::Start();
}