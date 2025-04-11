#include "Tests.h"
#include <iostream>
#include <ostream>
#include <typeindex>
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include "GleamECS/Runtime/System/System.h"
#include "GleamECS/Runtime/System/SystemAllocator.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamECS/Runtime/World/World.h"
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
    ASSERT_EQ(archetypeInfos[0], R"(Name:d1c9b3f452d6c5636f23b1a112880c3b
ID:d1c9b3f4-52d6-c563-6f23-b1a112880c3b
Size:28
Components:
struct RigidBody
4	12
struct SpringPhysics
16	12)");
    ASSERT_EQ(archetypeInfos[1], R"(Name:physicsWithSpringArchetype
ID:7e87fdd1-71de-8333-02ae-13c3525dd9c3
Size:32
Components:
struct Transform
4	4
struct RigidBody
8	12
struct SpringPhysics
20	12)");
    ASSERT_EQ(archetypeInfos[2], R"(Name:physicsArchetype
ID:87433cc8-8278-a9f6-f2eb-1d0d3628bfc6
Size:20
Components:
struct Transform
4	4
struct RigidBody
8	12)");

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


inline std::stringstream printResult = {};

template <int ID, class TGroup = void, auto... Args>
class PrintSystem : public System<TGroup, Args...>
{
public:
    static void Update(EntityAllocator& entities)
    {
        printResult << ID << "->Update\n";
    }
};
template <int ID, class TGroup = void, auto... Args>
class PrintSystemGroup : public System<TGroup, Args...>
{
public:
    static void Update(EntityAllocator& entities, const std::function<void(EntityAllocator&)>& updateGroup)
    {
        printResult << ID << "->Update\n";
        updateGroup(entities);
    }
};


TEST(ECS, SystemOrder)
{
    class System2 : public PrintSystem<2>
    {
    };
    class System3 : public PrintSystemGroup<3, System2, SystemRelation::After>
    {
    };
    class System3_2 : public PrintSystemGroup<32, System3>
    {
    };
    class System3_1 : public PrintSystem<31, System3_2, SystemRelation::Before>
    {
    };
    class System1 : public PrintSystem<1, System2, SystemRelation::Before>
    {
    };
    class System3_3 : public PrintSystem<33, System3_2, SystemRelation::After>
    {
    };
    class System3_2_2 : public PrintSystem<322, System3_2>
    {
    };
    class System3_2_1 : public PrintSystem<321, System3_2_2, SystemRelation::Before>
    {
    };

    ///- system1
    ///- system2
    ///- system3
    ///  - system3_1
    ///  - system3_2
    ///    - system3_2_1
    ///    - system3_2_2
    ///  - system3_3

    EntityInfoAllocator entitiesInfoAllocator;
    EntityAllocator entities;
    SystemAllocator systems;
    systems.AddSystem<System2>();
    systems.AddSystem<System3>();
    systems.AddSystem<System3_1>();
    systems.AddSystem<System1>();
    systems.AddSystem<System3_3>();
    systems.AddSystem<System3_2_2>();
    systems.AddSystem<System3_2_1>();
    systems.Update(entities);

    std::cout << printResult.str() << std::endl;
    ASSERT_EQ(printResult.str(), R"(1->Update
2->Update
3->Update
31->Update
32->Update
321->Update
322->Update
33->Update
)");
}

TEST(ECS, View)
{
    EntityAllocator entities = {};

    Entity physicsEntity = entities.AddEntity(physicsArchetype);
    Entity physicsWithSpring = entities.AddEntity(physicsWithSpringArchetype);

    View<Transform, RigidBody>::Each(entities, [](auto& transform, auto&)
    {
        ++transform.position;
    });
    ASSERT_EQ(entities.GetComponent<Transform>(physicsEntity).position, 1);
    ASSERT_EQ(entities.GetComponent<Transform>(physicsWithSpring).position, 1);

    View<QueryExclusion<SpringPhysics>, Transform, RigidBody>::Each(entities, [](auto& transform, auto&)
    {
        ++transform.position;
    });
    ASSERT_EQ(entities.GetComponent<Transform>(physicsEntity).position, 2);
    ASSERT_EQ(entities.GetComponent<Transform>(physicsWithSpring).position, 1);

    entities.RemoveEntity(physicsEntity);
    entities.RemoveEntity(physicsWithSpring);
}


/**
 * 质点弹簧物理系统模拟：https://zhuanlan.zhihu.com/p/361126215
 */
class PhysicsSystem : public System<>
{
public:
    constexpr static float DeltaTime = 0.02f;

    static void Update(EntityAllocator& entities)
    {
        View<Transform, RigidBody>::Each(entities, [](Transform& transform, RigidBody& rigidBody)
        {
            float acceleration = rigidBody.force / rigidBody.mass; //牛顿第二定律
            acceleration += rigidBody.mass * -9.8f; //添加重力加速度
            rigidBody.velocity += acceleration * DeltaTime;
            transform.position += rigidBody.velocity * DeltaTime;
            rigidBody.force = 0;
        });
        View<Transform, RigidBody, SpringPhysics>::Each(entities, [](Transform& transform, RigidBody& rigidBody, SpringPhysics& spring)
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
    SystemAllocator systems = {};
    systems.AddSystem<PhysicsSystem>();
    EntityAllocator entities = {};
    for (int i = 0; i < 10; i++)
        entities.AddEntity(i % 2 == 0 ? physicsArchetype : physicsWithSpringArchetype);

    for (int i = 0; i < 200; i++)
        systems.Update(entities); //更新

    std::stringstream ss;
    View<Transform>::Each(entities, [&entities,&ss](const Entity entity, Transform& transform)
    {
        ss << std::format("{:10.3f}", transform.position) << '|';
        ASSERT_TRUE(entities.HasComponent<SpringPhysics>(entity)
            ?abs(transform.position+5) < 0.1f
            :transform.position<70);
    });
    std::cout << ss.str();
}


// TEST(ECS, World)
// {
//     World world;
//     Entity entities[2] = {
//         world.GetEntityAllocator().AddEntity(physicsArchetype),
//         world.GetEntityAllocator().AddEntity(physicsArchetype),
//     };
//     world.RemoveEntityAsync(entities[0]);
//     world.GetEntityAllocator().MoveEntity(entities[1], physicsWithSpringArchetype);
//
//     View view = View<Transform, RigidBody, SpringPhysics>(world.GetEntityAllocator());
//     view.Each([entities](auto& entity, auto& transform, auto& rigidBody, auto& spring)
//     {
//         ASSERT_EQ(entity, entities[1]);
//         ASSERT_EQ(transform, Transform());
//         ASSERT_EQ(rigidBody, RigidBody());
//         ASSERT_EQ(spring, SpringPhysics());
//     });
//
//     RigidBody inRigidBody = {100, 1, 2};
//     SpringPhysics inSpring = {1, 2, 3};
//     world.GetEntityAllocator().SetComponents(entities[1], inRigidBody, inSpring);
//     RigidBody outRigidBody;
//     SpringPhysics outSpring;
//     world.GetEntityAllocator().GetComponents(entities[1], outRigidBody, outSpring);
//     ASSERT_EQ(outRigidBody, inRigidBody);
//     ASSERT_EQ(outSpring, inSpring);
//
//     entities[0] = world.GetEntityAllocator().AddEntity(physicsArchetype);
//     world.GetEntityAllocator().SetComponents(entities[0], Transform{3});
//     ASSERT_EQ(world.GetEntityAllocator().GetComponent<Transform>(entities[0]), Transform{3});
//
//     world.RemoveEntityAsync(entities[0]);
//     world.RemoveEntityAsync(entities[1]);
//     world.Update();
// }


//
// TEST(ECS, Scene)
// {
//     class TestSystem : public SystemT<>
//     {
//         View<Transform> view = {};
//
//         void Start() override
//         {
//             view = {World::GetCurrentWorld().GetEntityAllocator()};
//             view.Each([](Transform& transform)
//             {
//                 transform.position++;
//             });
//         }
//         void Update() override
//         {
//             view.Each([](Transform& transform)
//             {
//                 transform.position++;
//             });
//         }
//         void Stop() override
//         {
//             view.Each([](Transform& transform)
//             {
//                 transform.position--;
//             });
//         }
//     };
//
//     Archetype& archetype = Archetype::CreateOrGet({TransformType});
//
//     World world;
//     Scene& scene = world.AddScene("TestScene", true);
//     Entity entity = world.AddEntity(archetype);
//     System& system = world.AddSystem<TestSystem>();
//
//     world.Update();
//     ASSERT_EQ(world.GetEntityAllocator().GetComponent<Transform>(entity).position, 2);
//     world.Update();
//     ASSERT_EQ(world.GetEntityAllocator().GetComponent<Transform>(entity).position, 3);
//
//     scene.RemoveEntity(entity);
//     world.RemoveScene(scene);
//     ASSERT_EQ(world.GetEntityAllocator().GetComponent<Transform>(entity).position, 3);
//     world.Update();
//     ASSERT_EQ(world.GetEntityAllocator().GetComponent<Transform>(entity).position, 2);
//
//     world.Clear();
//     ASSERT_EQ(world.GetEntityInfoAllocator().HasEntity(entity), false);
// }