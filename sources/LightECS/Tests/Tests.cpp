#include <iostream>
#include <ostream>
#include <typeindex>
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include "LightECS/Runtime/Archetype.hpp"
#include "LightECS/Runtime/World.h"
#include "LightECS/Runtime/Heap.h"
#include "LightECS/Runtime/System.hpp"
#include "LightECS/Runtime/View.hpp"

struct Transform
{
    float position;

    friend bool operator==(const Transform& lhs, const Transform& rhs)
    {
        return lhs.position == rhs.position;
    }
};

struct RigidBody
{
    float force;
    float mass = 1;
    float velocity;

    friend bool operator==(const RigidBody& lhs, const RigidBody& rhs)
    {
        return lhs.force == rhs.force
            && lhs.mass == rhs.mass
            && lhs.velocity == rhs.velocity;
    }
};

struct SpringPhysics
{
    float pinPosition = 0;
    float length = 5;
    float elasticity = 200;

    friend bool operator==(const SpringPhysics& lhs, const SpringPhysics& rhs)
    {
        return lhs.pinPosition == rhs.pinPosition
            && lhs.length == rhs.length
            && lhs.elasticity == rhs.elasticity;
    }
};

MakeArchetype(physicsArchetype, Transform, RigidBody)
MakeArchetype(physicsWithSpringArchetype, Transform, RigidBody, SpringPhysics)

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
                    container.erase(container.begin() + i);
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
    for (auto& archetype : Archetype::allArchetypes)
    {
        std::cout << archetype->ToString() << "\n";
        std::cout << archetype->size << "\n";
    }

    std::byte* data = static_cast<std::byte*>(malloc(physicsWithSpringArchetype.size));
    physicsWithSpringArchetype.RunConstructor(data);
    Transform& transform = *reinterpret_cast<Transform*>(data + physicsWithSpringArchetype.componentOffsets[1]);
    RigidBody& rigidBody = *reinterpret_cast<RigidBody*>(data + physicsWithSpringArchetype.componentOffsets[2]);
    SpringPhysics& spring = *reinterpret_cast<SpringPhysics*>(data + physicsWithSpringArchetype.componentOffsets[3]);
    ASSERT_EQ(transform, Transform());
    ASSERT_EQ(rigidBody, RigidBody());
    ASSERT_EQ(spring, SpringPhysics());
    free(data);
}

TEST(ECS, World)
{
    Entity entities[2];
    World::AddEntities(physicsArchetype, 2, entities);
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

    entities[0] = World::AddEntity(physicsArchetype, Transform{3});
    ASSERT_EQ(World::GetComponent<Transform>(entities[0]), Transform{3});
}

struct PhysicsSystem : System<MinSystemOrder, MaxSystemOrder>
{
    //质点弹簧物理系统模拟：https://zhuanlan.zhihu.com/p/361126215
    inline static float DeltaTime = 0.02f;

    static void Update()
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
    for (int i = 0; i < 10; i++)
        World::AddEntity(i % 2 == 0 ? physicsArchetype : physicsWithSpringArchetype);

    World::AddSystem<PhysicsSystem>();

    std::stringstream log;
    for (int i = 0; i < 200; i++)
    {
        //更新
        World::Update();

        //输出
        View<Transform>::Each([&log](Transform& transform)
        {
            log << std::format("{:10.3f}", transform.position) << '|';
        });
        std::cout << log.str() << "\n";
        log.str("");
    }
}
