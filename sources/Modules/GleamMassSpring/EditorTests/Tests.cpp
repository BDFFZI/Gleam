#include "GleamECS/Runtime/System.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Editor/System/MassSpringRenderingSystem.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"
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

        //碰撞弹性系数
        {
            Entity point = PhysicsSystem::AddParticle(float3{-10, 0, 0}, 0);
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
            Entity point = PhysicsSystem::AddParticle(float3{-10, 0, 10}, 0);
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
            Entity pointA = PhysicsSystem::AddParticle(float3{0, 0, 0});
            Entity pointB = PhysicsSystem::AddParticle(float3{5, 0, 0}, 0);

            FixedParticle(pointA);

            PhysicsSystem::AddSpring(pointA, pointB);
        }
        
        //软绳
        {
            Entity points[5];
            for (int i = 0; i < 5; i++)
                points[i] = PhysicsSystem::AddParticle(float3{i * 2.0f, 0, 10});

            FixedParticle(points[0]);

            for (int i = 0; i < 4; i++)
                PhysicsSystem::AddSpring(points[i], points[i + 1]);
        }

        //布料
        {
            //添加质点
            Entity particles[5][5];
            constexpr int length = 2;
            for (int i = 0, y = 0; i < 5; i++, y += length)
                for (int j = 0, x = 0; j < 5; j++, x += length)
                {
                    particles[i][j] = PhysicsSystem::AddParticle(
                        float3{static_cast<float>(x), 0, 20 + static_cast<float>(y)}
                    );
                }

            FixedParticle(particles[0][0]);
            FixedParticle(particles[0][4]);

            //添加弹簧
            for (int y = 0; y < 5; y++)
                for (int x = 0; x < 5; x++)
                {
                    //抗拉伸弹簧
                    if (x + 1 < 5)
                        PhysicsSystem::AddSpring(particles[y][x], particles[y][x + 1], 0.7f);
                    if (y + 1 < 5)
                        PhysicsSystem::AddSpring(particles[y][x], particles[y + 1][x], 0.7f);
                    //抗斜向拉伸弹簧
                    if (x + 1 < 5 && y + 1 < 5) //右下
                        PhysicsSystem::AddSpring(particles[y][x], particles[y + 1][x + 1], 0.4f);
                    if (x + 1 < 5 && y - 1 >= 0) //右上
                        PhysicsSystem::AddSpring(particles[y][x], particles[y - 1][x + 1], 0.4f);
                    //抗对折弹簧
                    if (x + 2 < 5)
                        PhysicsSystem::AddSpring(particles[y][x], particles[y][x + 2], 0.1f);
                    if (y + 2 < 5)
                        PhysicsSystem::AddSpring(particles[y][x], particles[y + 2][x], 0.1f);
                }
        }

        //碰撞
        for (int i = 1; i < 3; i++)
        {
            Entity collider = World::AddEntity(SphereCollider);
            World::SetComponents(collider, LocalTransform{float3{0, -10, i * 10.0f}}, Sphere{0, 5});
        }
    }
};
Gleam_MakeGlobalSystem(TestSystem)
Gleam_AddSystems(TestSystem)

#ifdef GleamEngineEditor
Gleam_AddEditorSystems(TestSystem)
#endif

Gleam_Main