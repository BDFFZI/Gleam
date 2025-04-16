#include "GleamECS/RuntimeTests/Tests.h"

#include <gtest/gtest.h>
#include <stduuid/uuid.h>

#include "GleamAssets/Runtime/SceneAssetBundle.h"
#include "GleamAssets/Runtime/Asset/PersistentEntity.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamAssets/Runtime/Asset/BasicSceneInfo.h"
#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

using namespace Gleam;

struct MyComponent
{
    int value;
    Entity dependency = Entity::Null;
};

Gleam_MakeTypeWithID(MyComponent, "")
{
    Gleam_MakeType_AddField(value);
    Gleam_MakeType_AddField(dependency);
}

class MySystem : public System<>
{
    void Update() override
    {
        World::GetView<MyComponent>().Each([](MyComponent& myComponent)
        {
            myComponent.value++;
        });
    }
};

class MySystem2 : public MySystem
{
};

// TEST(Assets, Scene)
void main()
{
    //测试场景的创建和保存
    {
        Scene& scene = World::AddScene("TestScene");
        //添加实体
        Entity entity = World::GetEntityAllocator().AddEntity(Transform{999}, RigidBody{}, SpringPhysics{});
        scene.AddEntity(entity);
        scene.AddEntity(World::GetEntityAllocator().AddEntity(Transform{2}, RigidBody{}));
        scene.AddEntity(World::GetEntityAllocator().AddEntity(MyComponent{3, entity}));
        //添加系统
        scene.AddSystem(SystemInfoAllocator::CreateOrGetSystemInfo<MySystem>());
        World::Update(); //应用世界更改
        //持久化
        AssetBundle& assetBundle = AssetBundle::Create();
        SceneAssetBundle::SaveToAssetBundle(scene, assetBundle);
        AssetBundle::SaveJson("TestScene.json", assetBundle);
        AssetBundle::Unload(assetBundle); //卸载资源包不影响，场景内实体
        ASSERT_EQ(World::GetView<Transform>().Count(), 2);
        //销毁场景会移除实体
        World::RemoveScene(scene);
        World::Update(); //应用世界更改
        ASSERT_EQ(World::GetView<Transform>().Count(), 0);
    }

    //测试加载实体场景
    {
        Query& query = Query::GetQuery<QueryAlways, Transform>();
        //加载资源包就会加载实体
        AssetBundle& assetBundle = AssetBundle::LoadJson("TestScene.json");
        World::Update(); //应用世界更改
        Query& query2 = Query::GetQuery<QueryAlways, Transform>();
        ASSERT_EQ(World::GetView<Transform>().Count(), 2);
        //场景通过读取资源包恢复数据
        Scene& scene = SceneAssetBundle::MoveFromAssetBundle(assetBundle);
        AssetBundle::Unload(assetBundle); //从资源包内恢复场景后资源包就可以直接删除了（如果不需要再次存储的话）。
        ASSERT_EQ(scene.GetEntities().size(), 3);
        ASSERT_TRUE(scene.HasSystem(SystemInfoAllocator::CreateOrGetSystemInfo<MySystem>() ));
        //验证场景内实体信息正确性
        std::vector<Entity> entities;
        World::GetView<MyComponent>().Fetch(entities);
        ASSERT_TRUE(scene.HasEntity(entities[0]));
        MyComponent& myComponent = World::GetEntityAllocator().GetComponent<MyComponent>(entities[0]);
        ASSERT_EQ(myComponent.value, 3);
        ASSERT_EQ(World::GetEntityAllocator().GetComponent<Transform>(myComponent.dependency).position, 999);

        World::RemoveScene(scene);
        World::Update(); //应用世界更改
    }

    //测试运行场景
    {
        //中途添加系统
        AssetBundle& assetBundle = AssetBundle::LoadJson("TestScene.json");
        Scene& scene = SceneAssetBundle::MoveFromAssetBundle(assetBundle);
        scene.AddSystem(SystemInfoAllocator::CreateOrGetSystemInfo<MySystem2>());
        ASSERT_EQ(scene.GetSystems().size(), 2);
        //启动场景
        ASSERT_EQ(World::GetSystemAllocator().GetRootSystem().GetSystems().size(), 0);
        scene.Start();
        World::Update();
        ASSERT_EQ(World::GetSystemAllocator().GetRootSystem().GetSystems().size(), 2);
        //停止场景
        scene.Stop();
        World::Update();
        ASSERT_EQ(World::GetSystemAllocator().GetRootSystem().GetSystems().size(), 0);
        //实体被更新
        ASSERT_EQ(World::GetEntityAllocator().GetComponent<MyComponent>(assetBundle.GetObject<PersistentEntity>(3).GetEntity()).value, 5);
        //写回资源包并卸载场景
        SceneAssetBundle::SaveToAssetBundle(scene, assetBundle);
        AssetBundle::SaveJson("TestScene.json", assetBundle);
        AssetBundle::Unload(assetBundle);
        World::RemoveScene(scene);
        World::Update(); //应用世界更改
    }

    {
        //重新加载
        AssetBundle& assetBundle = AssetBundle::LoadJson("TestScene.json");
        Scene& scene = SceneAssetBundle::MoveFromAssetBundle(assetBundle);
        AssetBundle::Unload(assetBundle);

        ASSERT_EQ(scene.GetEntities().size(), 3);
        ASSERT_EQ(scene.GetSystems().size(), 2);
        World::RemoveScene(scene);
    }

    World::Clear();
}

// class MySystem3 : public System<>
// {
//     void Start() override
//     {
//         View<MyComponent>().Each([](MyComponent& myComponent)
//         {
//             std::cout << myComponent.value << std::endl;
//         });
//     }
//     void Update() override
//     {
//         Engine::Stop();
//     }
//     void Stop() override
//     {
//         View<MyComponent>().Each([](MyComponent& myComponent)
//         {
//             std::cout << myComponent.value << std::endl;
//         });
//     }
// };
// Gleam_MakeRuntimeSystem(MySystem)
// Gleam_MakeRuntimeSystem(MySystem2)
//
// TEST(Assets, Runtime)
// {
//     uuids::uuid id = MD5("TestScene2").toArray();
//     //持久化一个场景
//     {
//         Scene& scene = World::AddScene("TestScene2");
//         scene.AddEntity(World::AddEntity(MyComponent{123}));
//         scene.AddSystem(GlobalMySystem);
//         scene.AddSystem(GlobalMySystem2);
//         AssetBundle& assetBundle = AssetBundle::Create(id);
//         SceneAssetBundle::SaveToAssetBundle(scene, assetBundle);
//         Resources::Create(assetBundle);
//         assetBundle.Unload(assetBundle);
//         World::RemoveScene(scene);
//     }
//
//     //加载场景
//     SceneManager::LoadScene(id);
//
//     Engine::Start();
// }