﻿#include <array>
#include <iostream>
#include <ostream>
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include "LightECS/Runtime/Heap.h"
#include "LightECS/Runtime/World.h"
#include <stdarg.h>
#include <unordered_set>

#include "LightECS/Runtime/System.h"

#ifdef BENCHMARK_Heap
struct Data
{
    float data[32];
};
void VectorTest(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<Data> container;
        container.resize(30);
        container.resize(60);
        int size = container.size();
        for (int i = 0; i < size; i++)
            container[i].data[0] = i;
        container.erase(container.begin(), container.begin() + 30);

        container.resize(90);
        container.resize(120);
        size = container.size();
        for (int i = 0; i < size; i++)
            container[i].data[1] = i;
        container.erase(container.begin() + 30, container.begin() + 60);

        for (int i = 0; i < container.size(); i++)
        {
            if (i % 2 == 0)
                container.erase(container.begin() + i);
        }
    }
}
void HeapTest(benchmark::State& state)
{
    for (auto _ : state)
    {
        Heap container(sizeof(Data));
        container.AddElements(30);
        container.AddElements(30);
        container.ForeachElements(0, container.GetCount(), [](const int index, std::byte* ptr)
        {
            Data* data = reinterpret_cast<Data*>(ptr);
            data->data[0] = index;
        });
        container.RemoveElements(0, 30);

        container.AddElements(30);
        container.AddElements(30);
        container.ForeachElements(0, container.GetCount(), [](const int index, std::byte* ptr)
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
}
BENCHMARK(VectorTest);
BENCHMARK(HeapTest);
BENCHMARK_MAIN();
#endif

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

struct Transform
{
    float position;
};

struct RigidBody
{
    float force;
    float mass = 1;
    float acceleration;
    float velocity;
};

struct Spring
{
    float pinPosition = 0;
    float length = 10;
    float elasticity = 1000;
};

class PhysicsSystem : public System
{
public:
    float deltaTime = 0.02f;

    void Update(World& world) const override
    {
        View<Transform, RigidBody> physicalEntities = {};
        physicalEntities.Each(world, [deltaTime = deltaTime](Transform& transform, RigidBody& rigidBody)
        {
            rigidBody.force += rigidBody.mass * -9.8f;

            float acceleration = rigidBody.force / rigidBody.mass;
            rigidBody.velocity = acceleration * deltaTime;
            rigidBody.force = 0;

            transform.position += rigidBody.velocity * deltaTime;
        });

        View<Transform, RigidBody, Spring> physicalElasticEntities = {};
        physicalElasticEntities.Each(world, [](Transform& transform, RigidBody& rigidBody, Spring& spring)
        {
            float vector = spring.pinPosition - transform.position;
            float direction = vector > 1 ? 1 : -1;
            float distance = abs(vector) - spring.length;
            float elasticForce = distance * spring.elasticity * direction;
            rigidBody.force += elasticForce;
        });
    }
};


MakeArchetype(physicsArchetype, Transform, RigidBody)
MakeArchetypeInherited(physicsWithSpringArchetype, physicsArchetype, Spring)

template <typename T>
concept Int = std::is_same_v<T, int>;

template <typename T>
concept Con = requires()
{
    sd
};

template <typename T>
    requires Int<T>
void Func(T value)
{
}

void main()
{
    Func(1);

    // for (auto& archetype : Archetype::allArchetypes)
    // {
    //     std::cout << archetype->ToString() << "\n";
    //     std::cout << archetype->GetSize() << "\n";
    // }

    World world;
    world.AddEntities(physicsWithSpringArchetypeID, 1);

    View<Transform, RigidBody, Spring> view;
    view.Each(world, [](auto& transform, auto& rigidBody, auto& spring)
    {
        getchar();
    });

    // World world;
    // for (int i = 0; i < 10; i++)
    //     world.AddEntity(i % 2 == 0 ? physicsArchetypeID : physicsWithSpringArchetypeID);
    //
    // PhysicsSystem system = {};
    // system.deltaTime = 0.01f;
    //
    // std::stringstream log;
    // while (true)
    // {
    //     system.Update(world);
    //
    //     View<Transform> showProcess = {};
    //     showProcess.Each(world, [&log](Transform& transform)
    //     {
    //         log << std::to_string(transform.position) << '|';
    //     });
    //
    //     std::cout << log.str() << std::endl;
    //     log.str("");
    // }
}
