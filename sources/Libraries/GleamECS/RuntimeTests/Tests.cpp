#include <iostream>
#include <ostream>
#include <typeindex>
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/World.h"
#include "GleamECS/Runtime/Heap.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamECS/Runtime/View.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

using namespace Gleam;

TEST(ECS, Heap)
{
    Heap heap(sizeof(int));

    heap.AddElements(5, [](const int itemIndex, std::byte* item)
    {
        int* element = reinterpret_cast<int*>(item);
        *element = itemIndex;
    });

    heap.RemoveElements(2, 2);
    heap.RemoveElement(1);
    heap.AddElement([](std::byte* item)
    {
        *reinterpret_cast<int*>(item) = 5;
    });
    *reinterpret_cast<int*>(heap.At(0)) = 3;

    std::vector<int> vector(heap.GetCount());
    heap.CopyTo(reinterpret_cast<std::byte*>(vector.data()), 0, heap.GetCount());
    ASSERT_EQ(vector.size(), 3);
    ASSERT_EQ(vector[0], 3);
    ASSERT_EQ(vector[1], 4);
    ASSERT_EQ(vector[2], 5);
}

TEST(ECS, HeapBenchmark)
{
    struct Data
    {
        size_t data[32];
    };

    benchmark::RegisterBenchmark("Vector", [](benchmark::State& state)
    {
        for (auto _ : state)
        {
            std::vector<Data> container;
            container.resize(30);
            container.resize(60);
            size_t size = container.size();
            for (size_t i = 0; i < size; i++)
                container[i].data[0] = i;
            container.erase(container.begin(), container.begin() + 30);

            container.resize(90);
            container.resize(120);
            size = container.size();
            for (size_t i = 0; i < size; i++)
                container[i].data[1] = i;
            container.erase(container.begin() + 30, container.begin() + 60);

            for (size_t i = 0; i < container.size(); i++)
            {
                if (i % 2 == 0)
                    container.erase(container.begin() + static_cast<int64_t>(i));
            }
        }
    });
    benchmark::RegisterBenchmark("Heap", [](benchmark::State& state)
    {
        for (auto _ : state)
        {
            Heap container(sizeof(Data));
            container.AddElements(30);
            container.AddElements(30);
            int index = 0;
            container.ForeachElements([&index](std::byte* ptr)
            {
                Data* data = reinterpret_cast<Data*>(ptr);
                data->data[0] = index;
            });
            container.RemoveElements(0, 30);

            container.AddElements(30);
            container.AddElements(30);
            index = 0;
            container.ForeachElements([&index](std::byte* ptr)
            {
                Data* data = reinterpret_cast<Data*>(ptr);
                data->data[1] = index;
            });
            container.RemoveElements(30, 30);

            for (int i = 0; i < container.GetCount(); i++)
            {
                if (i % 2 == 0)
                    container.RemoveElement(i);
            }
        }
    });
    benchmark::Initialize(nullptr, nullptr);
    benchmark::RunSpecifiedBenchmarks();
}

struct Transform
{
    float position;

    friend bool operator==(const Transform& lhs, const Transform& rhs)
    {
        return abs(lhs.position - rhs.position) < std::numeric_limits<float>::epsilon();
    }
};

Gleam_MakeType(Transform, "00000000-0001-435E-AE61-D12757C441AE")
{
    Gleam_MakeType_AddField(position);
}

struct RigidBody
{
    float force;
    float mass = 1;
    float velocity;

    friend bool operator==(const RigidBody& lhs, const RigidBody& rhs)
    {
        return abs(lhs.force - rhs.force) < std::numeric_limits<float>::epsilon()
            && abs(lhs.mass - rhs.mass) < std::numeric_limits<float>::epsilon()
            && abs(lhs.velocity - rhs.velocity) < std::numeric_limits<float>::epsilon();
    }
};

Gleam_MakeType(RigidBody, "00000000-0002-435E-AE61-D12757C441AE")
{
    Gleam_MakeType_AddField(force);
    Gleam_MakeType_AddField(mass);
    Gleam_MakeType_AddField(velocity);
}

struct SpringPhysics
{
    float pinPosition = 0;
    float length = 5;
    float elasticity = 200;

    friend bool operator==(const SpringPhysics& lhs, const SpringPhysics& rhs)
    {
        return abs(lhs.pinPosition - rhs.pinPosition) < std::numeric_limits<float>::epsilon()
            && abs(lhs.length - rhs.length) < std::numeric_limits<float>::epsilon()
            && abs(lhs.elasticity - rhs.elasticity) < std::numeric_limits<float>::epsilon();
    }
};

Gleam_MakeType(SpringPhysics, "00000000-0003-435E-AE61-D12757C441AE")
{
    Gleam_MakeType_AddField(pinPosition);
    Gleam_MakeType_AddField(length);
    Gleam_MakeType_AddField(elasticity);
}

Gleam_MakeArchetype(physicsArchetype, Transform, RigidBody)
Gleam_MakeArchetypeChild(physicsWithSpringArchetype, physicsArchetype, SpringPhysics)

TEST(ECS, Archetype)
{
    //运行时创建原型
    Archetype::CreateOrGet({Type::CreateOrGet<RigidBody>(), Type::CreateOrGet<SpringPhysics>()});
    Archetype& archetype = Archetype::CreateOrGet({Type::CreateOrGet<RigidBody>(), Type::CreateOrGet<SpringPhysics>(), Type::CreateOrGet<Transform>()});
    ASSERT_EQ(Archetype::GetAllArchetypes().size(), 3); //一个运行时原型已被硬编码创建，故应该是3个

    //验证原型信息
    std::string archetypeInfos[3];
    int i = 0;
    for (auto& item : Archetype::GetAllArchetypes() | UnwrapRef)
    {
        // std::cout << to_string(item) << "\n";
        archetypeInfos[i] = to_string(item);
        i++;
    }
    ASSERT_EQ(archetypeInfos[0], R"(physicsArchetype	20
enum Gleam::Entity       0     4
struct Transform         4     4
struct RigidBody         8    12)");
    ASSERT_EQ(archetypeInfos[1], R"(physicsWithSpringArchetype	32
enum Gleam::Entity       0     4
struct Transform         4     4
struct SpringPhysics     8    12
struct RigidBody        20    12)");
    ASSERT_EQ(archetypeInfos[2], R"(9703e63357584ed2a3aec2059f480457	28
enum Gleam::Entity       0     4
struct SpringPhysics     4    12
struct RigidBody        16    12)");

    //验证原型读写内存能力
    std::byte* data = static_cast<std::byte*>(malloc(archetype.GetSize()));
    archetype.Construct(data);
    Transform& transform = *reinterpret_cast<Transform*>(data + archetype.GetComponentOffset(typeid(Transform)));
    RigidBody& rigidBody = *reinterpret_cast<RigidBody*>(data + archetype.GetComponentOffset(typeid(RigidBody)));
    SpringPhysics& spring = *reinterpret_cast<SpringPhysics*>(data + archetype.GetComponentOffset(typeid(SpringPhysics)));
    ASSERT_EQ(transform, Transform());
    ASSERT_EQ(rigidBody, RigidBody());
    ASSERT_EQ(spring, SpringPhysics());
    free(data);
}

TEST(ECS, World)
{
    Entity entities[2] = {
        World::AddEntity(physicsArchetype),
        World::AddEntity(physicsArchetype),
    };
    World::RemoveEntity(entities[0]);
    World::MoveEntity(entities[1], physicsWithSpringArchetype);

    View<Transform, RigidBody, SpringPhysics>::Each([entities](auto& entity, auto& transform, auto& rigidBody, auto& spring)
    {
        ASSERT_EQ(entity, entities[1]);
        ASSERT_EQ(transform, Transform());
        ASSERT_EQ(rigidBody, RigidBody());
        ASSERT_EQ(spring, SpringPhysics());
    });

    RigidBody inRigidBody = {100, 1, 2};
    SpringPhysics inSpring = {1, 2, 3};
    World::SetComponents(entities[1], inRigidBody, inSpring);
    RigidBody outRigidBody;
    SpringPhysics outSpring;
    World::GetComponents(entities[1], outRigidBody, outSpring);
    ASSERT_EQ(outRigidBody, inRigidBody);
    ASSERT_EQ(outSpring, inSpring);

    entities[0] = World::AddEntity(physicsArchetype);
    World::SetComponents(entities[0], Transform{3});
    ASSERT_EQ(World::GetComponent<Transform>(entities[0]), Transform{3});

    World::RemoveEntity(entities[0]);
    World::RemoveEntity(entities[1]);
}

/**
 * 质点弹簧物理系统模拟：https://zhuanlan.zhihu.com/p/361126215
 */
class PhysicsSystem : public System
{
public:
    constexpr static float DeltaTime = 0.02f;

private:
    void Update() override
    {
        View<Transform, RigidBody>::Each([](Transform& transform, RigidBody& rigidBody)
        {
            float acceleration = rigidBody.force / rigidBody.mass; //牛顿第二定律
            acceleration += rigidBody.mass * -9.8f; //添加重力加速度
            rigidBody.velocity += acceleration * DeltaTime;
            transform.position += rigidBody.velocity * DeltaTime;
            rigidBody.force = 0;
        });

        View<Transform, RigidBody, SpringPhysics>::Each([](Transform& transform, RigidBody& rigidBody, SpringPhysics& spring)
        {
            float vector = spring.pinPosition - transform.position;
            float direction = vector >= 0 ? 1 : -1;
            float distance = abs(vector) - spring.length;
            float elasticForce = spring.elasticity * distance * direction; //弹力或推力
            float resistance = -0.01f * spring.elasticity * (rigidBody.velocity * direction) * direction; //弹簧内部阻力（不添加无法使弹簧稳定）
            rigidBody.force += elasticForce + resistance;
        });
    }
};

TEST(ECS, System)
{
    PhysicsSystem physicsSystem{};
    for (int i = 0; i < 10; i++)
        World::AddEntity(i % 2 == 0 ? physicsArchetype : physicsWithSpringArchetype);
    World::AddSystem(physicsSystem);

    for (int i = 0; i < 200; i++)
        World::Update(); //更新

    std::stringstream ss;
    View<Transform>::Each([&ss](const Entity entity, Transform& transform)
    {
        ss << std::format("{:10.3f}", transform.position) << '|';
        ASSERT_TRUE(World::HasComponent<SpringPhysics>(entity)
            ?abs(transform.position+5) < 0.1f
            :transform.position<70);
    });
    std::cout << ss.str();

    World::Clear();
}

inline std::stringstream printResult = {};
class PrintSystem : public System
{
public:
    PrintSystem(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group)
        : System(group, MinOrder, MaxOrder, name)
    {
    }
    PrintSystem(const std::string_view& name, System& system, const OrderRelation orderRelation)
        : System(system, orderRelation, name)
    {
    }


    void Start() override
    {
        printResult << GetName() << "->Start\n";
    }
    void Stop() override
    {
        printResult << GetName() << "->Stop\n";
    }
    void Update() override
    {
        printResult << GetName() << "->Update\n";
    }
};
class PrintSystemGroup : public SystemGroup
{
public:
    PrintSystemGroup(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group)
        : SystemGroup(group, MinOrder, MaxOrder, name)
    {
    }
    PrintSystemGroup(const std::string_view& name, System& system, const OrderRelation orderRelation)
        : SystemGroup(system, orderRelation, name)
    {
    }

private:
    void Start() override
    {
        printResult << GetName() << "->Start\n";
        SystemGroup::Start();
    }
    void Stop() override
    {
        SystemGroup::Stop();
        printResult << GetName() << "->Stop\n";
    }
    void Update() override
    {
        printResult << GetName() << "->Update\n";
        SystemGroup::Update();
    }
};

TEST(ECS, SystemOrder)
{
    ///- system1
    ///- system2
    ///- system3
    ///  - system3_1
    ///  - system3_2
    ///    - system3_2_1
    ///    - system3_2_2
    ///  - system3_3
    PrintSystem system2 = {"system2", std::nullopt};
    PrintSystemGroup system3 = {"system3", system2, OrderRelation::After};
    PrintSystemGroup system3_2 = {"system3_2", system3,};
    PrintSystem system3_1 = {"system3_1", system3_2, OrderRelation::Before,};
    PrintSystem system1 = {"system1", system2, OrderRelation::Before,};
    PrintSystem system3_3 = {"system3_3", system3_2, OrderRelation::After,};
    PrintSystem system3_2_2 = {"system3_2_2", system3_2};
    PrintSystem system3_2_1 = {"system3_2_1", system3_2_2, OrderRelation::Before,};

    World::AddSystem(system2);
    World::AddSystem(system3);
    World::AddSystem(system3_1);
    World::AddSystem(system1);
    World::AddSystem(system3_3);
    World::AddSystem(system3_2_2);
    World::AddSystem(system3_2_1);

    World::Update();
    World::Clear();

    ASSERT_EQ(printResult.str(), R"(system1->Start
system2->Start
system3->Start
system3_1->Start
system3_2->Start
system3_2_1->Start
system3_2_2->Start
system3_3->Start
system1->Update
system2->Update
system3->Update
system3_1->Update
system3_2->Update
system3_2_1->Update
system3_2_2->Update
system3_3->Update
system3_3->Stop
system3_2_2->Stop
system3_2_1->Stop
system3_2->Stop
system3_1->Stop
system3->Stop
system2->Stop
system1->Stop
)");
}

TEST(ECS, View)
{
    Entity physicsEntity = World::AddEntity(physicsArchetype);
    Entity physicsWithSpring = World::AddEntity(physicsWithSpringArchetype);
    View<Transform, RigidBody>::Each([](auto& transform, auto&)
    {
        ++transform.position;
    });
    ASSERT_EQ(World::GetComponent<Transform>(physicsEntity).position, 1);
    ASSERT_EQ(World::GetComponent<Transform>(physicsWithSpring).position, 1);

    View<ViewExclusion<SpringPhysics>, Transform, RigidBody>::Each([](auto& transform, auto&)
    {
        ++transform.position;
    });
    ASSERT_EQ(World::GetComponent<Transform>(physicsEntity).position, 2);
    ASSERT_EQ(World::GetComponent<Transform>(physicsWithSpring).position, 1);

    World::RemoveEntity(physicsEntity);
    World::RemoveEntity(physicsWithSpring);
}

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
        ASSERT_EQ(World::GetComponent<MyComponent>(assetBundle.GetData<EntityAsset>(3).GetEntity()).value, 4);
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
    }
}