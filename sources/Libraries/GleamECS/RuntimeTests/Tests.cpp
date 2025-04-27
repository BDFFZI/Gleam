#include "Tests.h"
#include <iostream>
#include <ostream>
#include <typeindex>
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include "GleamECS/Runtime/World.h"
#include "GleamECS/Runtime/System/System.h"
#include "GleamECS/Runtime/System/SystemAllocator.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

using namespace Gleam;


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
    ASSERT_EQ(archetypeInfos[0], R"(Name:d1c9b3f4-52d6-c563-6f23-b1a112880c3b
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

TEST(ECS, View)
{
    EntityAllocator entities = {};

    Entity physicsEntity = entities.AddEntity(physicsArchetype);
    Entity physicsWithSpring = entities.AddEntity(physicsWithSpringArchetype);

    View<Transform, RigidBody>(entities).Each([](auto& transform, auto&)
    {
        ++transform.position;
    });
    ASSERT_EQ(entities.GetComponent<Transform>(physicsEntity).position, 1);
    ASSERT_EQ(entities.GetComponent<Transform>(physicsWithSpring).position, 1);

    View<QueryExclusion<SpringPhysics>, Transform, RigidBody>(entities).Each([](auto& transform, auto&)
    {
        ++transform.position;
    });
    ASSERT_EQ(entities.GetComponent<Transform>(physicsEntity).position, 2);
    ASSERT_EQ(entities.GetComponent<Transform>(physicsWithSpring).position, 1);

    entities.RemoveEntity(physicsEntity);
    entities.RemoveEntity(physicsWithSpring);
}


inline std::stringstream printResult = {};

template <int ID, class Parent>
class PrintSystem : public Parent
{
    void Start() override
    {
        printResult << ID << "->Create\n";
    }
    void Update() override
    {
        printResult << ID << "->Update\n";
    }
    void Stop() override
    {
        printResult << ID << "->Stop\n";
    }
};
template <int ID, class Parent>
class PrintSystemGroup : public Parent, public ISystemGroup
{
    void Start() override
    {
        printResult << ID << "->Create\n";
        ISystemGroup::Start();
    }
    void Update() override
    {
        printResult << ID << "->Update\n";
        ISystemGroup::Update();
    }
    void Stop() override
    {
        ISystemGroup::Stop();
        printResult << ID << "->Stop\n";
    }
};

TEST(ECS, SystemOrder)
{
    class System2 : public PrintSystem<2, System<>>
    {
    };
    class System3 : public PrintSystemGroup<3, RelativeSystem<System2, SystemRelation::After>>
    {
    };
    class System3_2 : public PrintSystemGroup<32, System<System3>>
    {
    };
    class System3_1 : public PrintSystem<31, RelativeSystem<System3_2, SystemRelation::Before>>
    {
    };
    class System1 : public PrintSystem<1, RelativeSystem<System2, SystemRelation::Before>>
    {
    };
    class System3_3 : public PrintSystem<33, RelativeSystem<System3_2, SystemRelation::After>>
    {
    };
    class System3_2_2 : public PrintSystem<322, System<System3_2>>
    {
    };
    class System3_2_1 : public PrintSystem<321, RelativeSystem<System3_2_2, SystemRelation::Before>>
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

    {
        SystemAllocator systems;
        systems.AddSystem<System2>();
        systems.AddSystem<System3>();
        systems.AddSystem<System3_1>();
        systems.AddSystem<System1>();
        systems.AddSystem<System3_3>();
        systems.AddSystem<System3_2_2>();
        systems.AddSystem<System3_2_1>();
        systems.Update();
    }

    std::cout << printResult.str() << '\n' << std::flush;
    ASSERT_EQ(printResult.str(), R"(1->Create
2->Create
3->Create
31->Create
32->Create
321->Create
322->Create
33->Create
1->Update
2->Update
3->Update
31->Update
32->Update
321->Update
322->Update
33->Update
33->Stop
322->Stop
321->Stop
32->Stop
31->Stop
3->Stop
2->Stop
1->Stop
)");
}

/**
 * 质点弹簧物理系统模拟：https://zhuanlan.zhihu.com/p/361126215
 */
class PhysicsSystem : public System<>
{
public:
    EntityAllocator* entities = {};
    float deltaTime = 0.02f;

    void Update() override
    {
        View<Transform, RigidBody>(*entities).Each([this](Transform& transform, RigidBody& rigidBody)
        {
            float acceleration = rigidBody.force / rigidBody.mass; //牛顿第二定律
            acceleration += rigidBody.mass * -9.8f; //添加重力加速度
            rigidBody.velocity += acceleration * deltaTime;
            transform.position += rigidBody.velocity * deltaTime;
            rigidBody.force = 0;
        });
        View<Transform, RigidBody, SpringPhysics>(*entities).Each([](Transform& transform, RigidBody& rigidBody, SpringPhysics& spring)
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
    EntityAllocator entities = {};
    for (int i = 0; i < 10; i++)
        entities.AddEntity(i % 2 == 0 ? physicsArchetype : physicsWithSpringArchetype);

    SystemAllocator systems = {};
    PhysicsSystem& physicsSystem = systems.AddSystem<PhysicsSystem>();
    physicsSystem.deltaTime = 0.02f;
    physicsSystem.entities = &entities;

    //更新
    for (int i = 0; i < 200; i++)
        systems.Update();

    std::stringstream ss;
    View<Transform>(entities).Each([&entities,&ss](const Entity entity, Transform& transform)
    {
        ss << std::format("{:10.3f}", transform.position) << '|';
        ASSERT_TRUE(entities.HasComponent<SpringPhysics>(entity)
            ?abs(transform.position+5) < 0.1f
            :transform.position<70);
    });
    std::cout << ss.str();
}

TEST(ECS, World)
{
    Entity entities[2] = {
        World::AddSceneEntity(physicsArchetype),
        World::AddSceneEntity(physicsArchetype),
    };
    World::RemoveSceneEntityAsync(entities[0]);
    World::MoveEntityAsync(entities[1], physicsWithSpringArchetype);

    World::Update();

    World::GetView<Transform, RigidBody, SpringPhysics>().Each([entities](auto& entity, auto& transform, auto& rigidBody, auto& spring)
    {
        ASSERT_EQ(entity, entities[1]);
        ASSERT_EQ(transform, Transform());
        ASSERT_EQ(rigidBody, RigidBody());
        ASSERT_EQ(spring, SpringPhysics());
    });

    RigidBody inRigidBody = {100, 1, 2};
    SpringPhysics inSpring = {1, 2, 3};
    World::GetEntityAllocator().SetComponents(entities[1], inRigidBody, inSpring);
    RigidBody outRigidBody;
    SpringPhysics outSpring;
    World::GetEntityAllocator().GetComponents(entities[1], outRigidBody, outSpring);
    ASSERT_EQ(outRigidBody, inRigidBody);
    ASSERT_EQ(outSpring, inSpring);

    entities[0] = World::GetEntityAllocator().AddEntity(physicsArchetype);
    World::GetEntityAllocator().SetComponents(entities[0], Transform{3});
    ASSERT_EQ(World::GetEntityAllocator().GetComponent<Transform>(entities[0]), Transform{3});

    World::Clear();
}


TEST(ECS, World2)
{
    class TestSystem : public System<>
    {
        void Start() override
        {
            World::GetView<Transform>().Each([](Transform& transform)
            {
                transform.position++;
            });
        }
        void Update() override
        {
            World::GetView<Transform>().Each([](Transform& transform)
            {
                transform.position++;
            });
        }
        void Stop() override
        {
            World::GetView<Transform>().Each([](Transform& transform)
            {
                transform.position--;
            });
        }
    };


    Scene& scene = World::AddScene("TestScene", true);
    Entity entity = World::AddSceneEntity(Archetype::CreateOrGet<Transform>("Transform"));
    ASSERT_EQ(World::GetEntityAllocator().GetComponent<Transform>(entity).position, 0);
    World::AddSceneSystem<TestSystem>();
    World::Update();
    ASSERT_EQ(World::GetEntityAllocator().GetComponent<Transform>(entity).position, 2);

    //回收场景
    scene.RemoveEntity(entity); //实体不回收
    World::RemoveScene(scene);

    World::Update();
    ASSERT_EQ(World::GetEntityAllocator().GetComponent<Transform>(entity).position, 1);

    World::AddSceneSystem<TestSystem>();
    World::Update();
    ASSERT_EQ(World::GetEntityAllocator().GetComponent<Transform>(entity).position, 3);

    World::Clear();
}