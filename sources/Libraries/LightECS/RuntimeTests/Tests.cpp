#include <iostream>
#include <ostream>
#include <typeindex>
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include "LightECS/Runtime/Archetype.h"
#include "LightECS/Runtime/World.h"
#include "LightECS/Runtime/Heap.h"
#include "LightECS/Runtime/View.h"
#include "LightMath/Runtime/LinearAlgebra/VectorMath.h"

using namespace Light;

struct Transform
{
    float position;

    friend bool operator==(const Transform& lhs, const Transform& rhs)
    {
        return abs(lhs.position - rhs.position) < std::numeric_limits<float>::epsilon();
    }
};

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

Light_MakeArchetype(physicsArchetype, Transform, RigidBody)
Light_MakeArchetypeChild(physicsWithSpringArchetype, physicsArchetype, SpringPhysics)

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

TEST(ECS, Archetype)
{
    for (auto& archetype : Archetype::GetAllArchetypes())
    {
        std::cout << archetype->ToString() << "\n";
        std::cout << archetype->GetArchetypeSize() << "\n";
    }

    std::byte* data = static_cast<std::byte*>(malloc(physicsWithSpringArchetype->GetArchetypeSize()));
    physicsWithSpringArchetype->RunConstructor(data);
    Transform& transform = *reinterpret_cast<Transform*>(data + physicsWithSpringArchetype->GetComponentOffset(1));
    RigidBody& rigidBody = *reinterpret_cast<RigidBody*>(data + physicsWithSpringArchetype->GetComponentOffset(2));
    SpringPhysics& spring = *reinterpret_cast<SpringPhysics*>(data + physicsWithSpringArchetype->GetComponentOffset(3));
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
    World::GetComponents(entities[1], &outRigidBody, &outSpring);
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

    PhysicsSystem(): System(std::nullopt, 0)
    {
    }

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
    World::AddSystem(&physicsSystem);

    World::Start();
    for (int i = 0; i < 200; i++)
        World::Update(); //更新
    World::Stop();

    std::stringstream ss;
    View<Transform>::Each([&ss](const Entity entity, Transform& transform)
    {
        ss << std::format("{:10.3f}", transform.position) << '|';
        ASSERT_TRUE(World::HasComponent<SpringPhysics>(entity)
            ?abs(transform.position+5) < 0.1f
            :transform.position<70);
    });
    std::cout << ss.str();
}

inline std::stringstream printResult = {};
class PrintSystem : public System
{
public:
    PrintSystem(const std::optional<SystemGroup*> group, const int minOrder, const int maxOrder, const char* name)
        : System(group, minOrder, maxOrder), name(name)
    {
    }
    PrintSystem(System* system, const OrderRelation orderRelation, const char* name)
        : System(system, orderRelation), name(name)
    {
    }

private:
    const char* name;

    void Start() override
    {
        printResult << name << "->Start\n";
    }
    void Stop() override
    {
        printResult << name << "->Stop\n";
    }
    void Update() override
    {
        printResult << name << "->Update\n";
    }
};
class PrintSystemGroup : public SystemGroup
{
public:
    PrintSystemGroup(const std::optional<SystemGroup*> group, const int minOrder, const int maxOrder, const char* name)
        : SystemGroup(group, minOrder, maxOrder), name(name)
    {
    }
    PrintSystemGroup(System* system, const OrderRelation orderRelation, const char* name)
        : SystemGroup(system, orderRelation), name(name)
    {
    }

private:
    const char* name;

    void Start() override
    {
        printResult << name << "->Start\n";
        SystemGroup::Start();
    }
    void Stop() override
    {
        SystemGroup::Stop();
        printResult << name << "->Stop\n";
    }
    void Update() override
    {
        printResult << name << "->Update\n";
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
    PrintSystem system2 = {std::nullopt, System::LeftOrder, System::RightOrder, "system2"};
    PrintSystemGroup system3 = {&system2, OrderRelation::After, "system3"};
    PrintSystemGroup system3_2 = {&system3, System::LeftOrder, System::RightOrder, "system3_2"};
    PrintSystem system3_1 = {&system3_2, OrderRelation::Before, "system3_1"};
    PrintSystem system1 = {&system2, OrderRelation::Before, "system1"};
    PrintSystem system3_3 = {&system3_2, OrderRelation::After, "system3_3"};
    PrintSystem system3_2_2 = {&system3_2, System::LeftOrder, System::RightOrder, "system3_2_2"};
    PrintSystem system3_2_1 = {&system3_2_2, OrderRelation::Before, "system3_2_1"};

    World::AddSystem(&system2);
    World::AddSystem(&system3);
    World::AddSystem(&system3_1);
    World::AddSystem(&system1);
    World::AddSystem(&system3_3);
    World::AddSystem(&system3_2_2);
    World::AddSystem(&system3_2_1);

    World::Start();
    World::Update();

    World::RemoveSystem(&system2);
    World::RemoveSystem(&system3);
    World::RemoveSystem(&system3_1);
    World::RemoveSystem(&system1);
    World::RemoveSystem(&system3_3);
    World::RemoveSystem(&system3_2_2);
    World::RemoveSystem(&system3_2_1);

    World::Stop();

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

TEST(ECS, SceneVisibility)
{
    Scene sleepScene = Scene{"Sleep"};
    Entity entity = sleepScene.AddEntity(physicsArchetype);
    World::SetComponents(entity, Transform{3});

    //显式指明场景，+1
    View<Transform>::Each([](Transform& transform)
    {
        transform.position++;
    }, &sleepScene);
    float2 position = World::GetComponent<Transform>(entity).position;
    ASSERT_TRUE(all(position == float2(4)));

    //不显式指明场景，==
    View<Transform>::Each([](Transform& transform)
    {
        transform.position++;
    });
    position = World::GetComponent<Transform>(entity).position;
    ASSERT_TRUE(all(position == float2(4)));

    //恢复到默认场景，+1
    Scene::MoveEntity(entity, World::GetMainScene());
    View<Transform>::Each([](Transform& transform)
    {
        transform.position++;
    });
    position = World::GetComponent<Transform>(entity).position;
    ASSERT_TRUE(all(position == float2(5)));

    World::GetMainScene()->RemoveEntity(entity);
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