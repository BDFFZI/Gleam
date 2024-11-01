#include <iostream>
#include <ostream>
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include "LightECS/Runtime/Heap.h"
#include "LightECS/Runtime/World.h"
#include <stdarg.h>
#include <unordered_set>

#include "LightECS/Runtime/System.h"
#include "LightUtility/Runtime/Chronograph.h"


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

struct Spring
{
    float pinPosition = 0;
    float length = 5;
    float elasticity = 200;

    friend bool operator==(const Spring& lhs, const Spring& rhs)
    {
        return lhs.pinPosition == rhs.pinPosition
            && lhs.length == rhs.length
            && lhs.elasticity == rhs.elasticity;
    }
};

MakeArchetype(physicsArchetype, Transform, RigidBody)
MakeArchetypeInherited(physicsWithSpringArchetype, physicsArchetype, Spring)

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
        float data[32];
    };

    benchmark::RegisterBenchmark("Vector", [](benchmark::State& state)
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
        std::cout << archetype->GetSize() << "\n";
    }

    std::byte* data = static_cast<std::byte*>(malloc(physicsWithSpringArchetype->GetSize()));
    physicsWithSpringArchetype->RunConstructor(data);
    Transform& transform = *reinterpret_cast<Transform*>(data + physicsWithSpringArchetype->GetComponentOffsets()[0]);
    RigidBody& rigidBody = *reinterpret_cast<RigidBody*>(data + physicsWithSpringArchetype->GetComponentOffsets()[1]);
    Spring& spring = *reinterpret_cast<Spring*>(data + physicsWithSpringArchetype->GetComponentOffsets()[2]);
    ASSERT_EQ(transform, Transform());
    ASSERT_EQ(rigidBody, RigidBody());
    ASSERT_EQ(spring, Spring());
    free(data);
}

TEST(ECS, WorldAndView)
{
    World world;
    world.AddEntities(physicsWithSpringArchetypeID, 1);

    View<Transform, RigidBody, Spring> view;
    view.Each(world, [](auto& transform, auto& rigidBody, auto& spring)
    {
        ASSERT_EQ(transform, Transform());
        ASSERT_EQ(rigidBody, RigidBody());
        ASSERT_EQ(spring, Spring());
    });
}

class PhysicsSystem : public System
{
    //质点弹簧物理系统模拟：https://zhuanlan.zhihu.com/p/361126215
public:
    float deltaTime;

    void Update(World& world) const override
    {
        View<Transform, RigidBody> physicalEntities = {};
        physicalEntities.Each(world, [deltaTime = deltaTime](Transform& transform, RigidBody& rigidBody)
        {
            float acceleration = rigidBody.force / rigidBody.mass; //牛顿第二定律
            acceleration += rigidBody.mass * -9.8f; //添加重力加速度
            rigidBody.velocity += acceleration * deltaTime;
            transform.position += rigidBody.velocity * deltaTime;
            rigidBody.force = 0;
        });

        View<Transform, RigidBody, Spring> physicalElasticEntities = {};
        physicalElasticEntities.Each(world, [](Transform& transform, RigidBody& rigidBody, Spring& spring)
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

// TEST(ECS, System)
void main()
{
    World world;
    for (int i = 0; i < 10; i++)
        world.AddEntity(i % 2 == 0 ? physicsArchetypeID : physicsWithSpringArchetypeID);

    PhysicsSystem physicsSystem = {};
    physicsSystem.deltaTime = 0.02f;

    std::stringstream log;
    for (int i = 0; i < 200; i++)
    {
        //更新
        physicsSystem.Update(world);

        //输出
        View<Transform> transformView = {};
        transformView.Each(world, [&log](Transform& transform)
        {
            log << std::format("{:10.3f}", transform.position) << '|';
        });
        std::cout << log.str() << "\n";
        log.str("");
    }
}
