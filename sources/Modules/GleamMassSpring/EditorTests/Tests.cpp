#include "GleamECS/Runtime/System.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Editor/System/MassSpringRenderingSystem.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"
#include "GleamMassSpring/Runtime/System/ForceSystem.h"
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

using namespace Gleam;

class TestSystem : public System
{
    static void FixedParticle(const Entity entity)
    {
        World::GetComponent<Particle>(entity).drag = 1;
    }

    void Start() override
    {
        MassSpringRenderingSystem.SetIsEnabled(true);
        ForceSystem.SetGravity(float3{0.0f, -9.81f, 0.0f} * 2);

        //碰撞弹性系数
        {
            PhysicsSystem::AddParticle(float3{-10, 0, 0}, 0);
            Entity collider = World::AddEntity(CuboidCollider);
            World::SetComponents(
                collider,
                LocalTransform{float3{-10, -10, 0}, Quaternion::Euler(float3{0, 0, 0})},
                Collider{0, 1},
                Cuboid::Create(0, 10)
            );
        }

        //碰撞摩擦系数
        {
            PhysicsSystem::AddParticle(float3{-10, 0, 10}, 0);
            Entity collider = World::AddEntity(CuboidCollider);
            World::SetComponents(
                collider,
                LocalTransform{float3{-10, -10, 10}, Quaternion::Euler(float3{40, 0, 0})},
                Collider{1, 0},
                Cuboid::Create(0, 10)
            );
        }

        //弹簧
        {
            Entity pointA = PhysicsSystem::AddParticle(float3{-10, 0, 20});
            Entity pointB = PhysicsSystem::AddParticle(float3{-15, 0, 20}, 0);

            FixedParticle(pointA);

            PhysicsSystem::AddSpring(pointA, pointB);
        }

        //软绳
        {
            //添加质点
            Entity points[20];
            for (int i = 0; i < std::size(points); i++)
                points[i] = PhysicsSystem::AddParticle(float3(15 + i, 0, 0));
            FixedParticle(points[0]);
            for (int i = 0; i < std::size(points) - 1; i++)
            {
                PhysicsSystem::AddSpring(points[i], points[i + 1], 1);
            }
        }

        //布料
        {
            //添加质点
            constexpr int size = 20;
            Entity points[size][size];
            for (int i = 0, y = 0; i < size; i++, y += 1)
                for (int j = 0, x = 0; j < size; j++, x += 1)
                    points[i][j] = PhysicsSystem::AddParticle(float3(15 + x, y, 10));
            FixedParticle(points[size - 1][0]);
            FixedParticle(points[size - 1][size - 1]);
            //添加弹簧
            for (int y = 0; y < size; y++)
                for (int x = 0; x < size; x++)
                {
                    //抗拉伸结构
                    if (x + 1 < size)
                        PhysicsSystem::AddSpring(points[y][x], points[y][x + 1]);
                    if (y + 1 < size)
                        PhysicsSystem::AddSpring(points[y][x], points[y + 1][x]);
                    //抗斜向拉伸结构
                    if (x + 1 < size && y + 1 < size) //右下
                        PhysicsSystem::AddSpring(points[y][x], points[y + 1][x + 1]);
                    if (x + 1 < size && y - 1 >= 0) //右上
                        PhysicsSystem::AddSpring(points[y][x], points[y - 1][x + 1]);
                    //抗弯折弹簧
                    if (x + 2 < size)
                        PhysicsSystem::AddSpring(points[y][x], points[y][x + 2]);
                    if (y + 2 < size)
                        PhysicsSystem::AddSpring(points[y][x], points[y + 2][x]);
                }
        }

        //碰撞
        for (int i = 0; i < 2; i++)
        {
            Entity collider = World::AddEntity(SphereCollider);
            World::SetComponents(collider, LocalTransform{float3{10, -10, i * 10.0f}}, Sphere{0, 5});
        }
    }
};
Gleam_MakeGlobalSystem(TestSystem)
Gleam_AddSystems(TestSystem)

#ifdef GleamEngineEditor
Gleam_AddEditorSystems(TestSystem)
#endif

Gleam_Main