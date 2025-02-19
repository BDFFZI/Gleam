#include "GleamECS/Runtime/System.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

using namespace Gleam;

class TestSystem : public System
{
    void Start() override
    {
        MassPointPhysics fixedPoint = {};
        fixedPoint.drag = 1;

        //质点和弹力碰撞
        {
            Entity point = PhysicsSystem::AddMassPoint(float3{-10, 0, 0});
            World::SetComponents(point, MassPointPhysics{1, 0});
            Entity collider = World::AddEntity(CuboidCollider);
            World::SetComponents(
                collider,
                LocalTransform{float3{-10, -10, 0}},
                Collider{0, 1},
                Cuboid::Create(0, 10)
            );
        }


        //弹簧
        {
            Entity pointA = PhysicsSystem::AddMassPoint(float3{0, 0, 0});
            Entity pointB = PhysicsSystem::AddMassPoint(float3{10, 0, 0});

            World::SetComponents(pointA, fixedPoint);

            PhysicsSystem::AddSpring(pointA, pointB);
        }

        //软绳
        {
            Entity points[5];
            for (int i = 0; i < 5; i++)
                points[i] = PhysicsSystem::AddMassPoint(float3{i * 2.0f, 0, 10});

            World::SetComponents(points[0], fixedPoint);

            for (int i = 0; i < 4; i++)
                PhysicsSystem::AddSpring(points[i], points[i + 1]);
        }

        //布料
        {
            //添加质点
            Entity massPoints[5][5];
            constexpr int length = 2;
            for (int i = 0, y = 0; i < 5; i++, y += length)
                for (int j = 0, x = 0; j < 5; j++, x += length)
                {
                    massPoints[i][j] = PhysicsSystem::AddMassPoint(
                        float3{static_cast<float>(x), 0, 20 + static_cast<float>(y)}
                    );
                }

            World::SetComponents(massPoints[0][0], fixedPoint);
            World::SetComponents(massPoints[0][4], fixedPoint);

            //添加弹簧
            for (int y = 0; y < 5; y++)
                for (int x = 0; x < 5; x++)
                {
                    //横纵结构
                    if (x + 1 < 5)
                        PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y][x + 1]);
                    if (y + 1 < 5)
                        PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y + 1][x]);
                    //斜向结构
                    if (x + 1 < 5 && y + 1 < 5) //右下
                        PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y + 1][x + 1], 1500);
                    if (x + 1 < 5 && y - 1 >= 0) //右上
                        PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y - 1][x + 1], 1500);
                    //抗弯折弹簧
                    if (x + 2 < 5)
                        PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y][x + 2], 500);
                    if (y + 2 < 5)
                        PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y + 2][x], 500);
                }
        }

        //碰撞
        for (int i = 0; i < 3; i++)
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