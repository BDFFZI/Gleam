#include "GleamECS/RuntimeTests/Tests.h"

#include <gtest/gtest.h>
#include <stduuid/uuid.h>

#include "GleamAssets/Runtime/Scene.h"
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/System.h"
#include "GleamECS/Runtime/View.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

using namespace Gleam;

// void Start() override
// {
//     Scene& scene = Scene::Create("Test");
//     Entity entity = World::AddEntity(DataArchetype);
//     World::SetComponents(entity, data);
//     scene.AddEntity(entity);
// }

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
Gleam_MakeGlobalSystem(MySystem)

class MySystem2 : public System
{
};
Gleam_MakeGlobalSystem(MySystem2)

TEST(ECS, Scene)
{
    //测试场景的创建和保存
    {
        Scene& scene = Scene::Create("TestScene");
        //添加实体
        Entity entity = World::AddEntity(Transform{999}, RigidBody{}, SpringPhysics{});
        scene.AddEntity(entity);
        scene.AddEntity(World::AddEntity(Transform{2}, RigidBody{}));
        scene.AddEntity(World::AddEntity(MyComponent{3, entity}));
        //添加系统
        scene.AddSystem(GlobalMySystem);
        //持久化
        AssetBundle& assetBundle = AssetBundle::Create();
        Scene::ToAssetBundle(scene, assetBundle);
        AssetBundle::SaveJson("TestScene.json", assetBundle);
        AssetBundle::UnLoad(assetBundle);
        //卸载资源包不影响，场景内实体
        ASSERT_EQ(View<Transform>::Count(), 2);
        //销毁场景会移除实体
        Scene::Destroy(scene);
        ASSERT_EQ(View<Transform>::Count(), 0);
    }

    //测试加载实体场景
    {
        //加载资源包就会加载实体
        AssetBundle& assetBundle = AssetBundle::LoadJson("TestScene.json");
        ASSERT_EQ(View<Transform>::Count(), 2);
        //场景通过读取资源包恢复数据
        Scene& scene = Scene::FromAssetBundle(assetBundle);
        AssetBundle::UnLoad(assetBundle); //从资源包内恢复场景后资源包就可以直接删除了（如果不需要再次存储的话）。
        ASSERT_EQ(scene.GetEntities().size(), 3);
        ASSERT_TRUE(scene.HasSystem(GlobalMySystem));
        //验证场景内实体信息正确性
        std::vector<Entity> entities;
        View<MyComponent>::Fetch(entities);
        ASSERT_TRUE(scene.HasEntity(entities[0]));
        MyComponent& myComponent = World::GetComponent<MyComponent>(entities[0]);
        ASSERT_EQ(myComponent.value, 3);
        ASSERT_EQ(World::GetComponent<Transform>(myComponent.dependency).position, 999);

        Scene::Destroy(scene);
    }

    //测试运行场景
    {
        //中途添加系统
        AssetBundle& assetBundle = AssetBundle::LoadJson("TestScene.json");
        Scene& scene = Scene::FromAssetBundle(assetBundle);
        scene.AddSystem(GlobalMySystem2);
        ASSERT_EQ(scene.GetSystems().size(), 2);
        //启动场景
        ASSERT_EQ(World::GetSystems().GetSubSystems().size(), 0);
        scene.Start();
        World::Update();
        ASSERT_EQ(World::GetSystems().GetSubSystems().size(), 2);
        //停止场景
        scene.Stop();
        World::Update();
        ASSERT_EQ(World::GetSystems().GetSubSystems().size(), 0);
        //实体被更新
        ASSERT_EQ(World::GetComponent<MyComponent>(assetBundle.GetObject<EntityAsset>(3).GetEntity()).value, 4);
        //写回资源包并卸载场景
        Scene::ToAssetBundle(scene, assetBundle);
        AssetBundle::SaveJson("TestScene.json", assetBundle);
        AssetBundle::UnLoad(assetBundle);
        Scene::Destroy(scene);
    }

    {
        //重新加载
        AssetBundle& assetBundle = AssetBundle::LoadJson("TestScene.json");
        Scene& scene = Scene::FromAssetBundle(assetBundle);
        AssetBundle::UnLoad(assetBundle);

        ASSERT_EQ(scene.GetEntities().size(), 3);
        ASSERT_EQ(scene.GetSystems().size(), 2);
        Scene::Destroy(scene);
    }
}