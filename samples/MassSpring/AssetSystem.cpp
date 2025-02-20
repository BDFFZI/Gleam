#include "AssetSystem.h"
#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/World.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"

Gleam::Entity AssetSystem::GetCameraEntity() const
{
    return cameraEntity;
}
void AssetSystem::Start()
{
    using namespace Gleam;
    //添加相机
    cameraEntity = World::AddEntity(CameraArchetype2);
    Camera& camera = World::GetComponent<Camera>(cameraEntity);
    camera.orthographic = true;
    camera.halfHeight = 50;
    //添加碰撞
    Entity collider1 = World::AddEntity(CuboidCollider);
    World::SetComponents(collider1, Cuboid{float3{-90, -100, 0}, float3{90, -50, 2}});
    Entity collider2 = World::AddEntity(CuboidCollider);
    World::SetComponents(collider2, Cuboid{float3{-100, -90, 0}, float3{-50, 90, 2}});
    Entity collider3 = World::AddEntity(CuboidCollider);
    World::SetComponents(collider3, Cuboid{float3{50, -90, 0}, float3{100, 90, 2}});
    Entity collider4 = World::AddEntity(CuboidCollider);
    World::SetComponents(collider4, Cuboid::Create(float3{0, 0, 0}, 25), LocalTransform{{0, -50, 2}, Quaternion::Euler({0, 0, 45})});
    colliders.emplace_back(collider1);
    colliders.emplace_back(collider2);
    colliders.emplace_back(collider3);
    colliders.emplace_back(collider4);
    //碰撞渲染器
    Entity colliderRenderer = World::AddEntity(LineRendererArchetype);
    LinesMesh& linesMesh = World::GetComponent<LinesMesh>(colliderRenderer);
    linesMesh.lines.insert(
        linesMesh.lines.end(),
        {
            Segment{float3{-17.67f, -50.0f, 1.0f}, float3{0, -50.0f + 17.67f, 1.0f}},
            Segment{float3{17.67f, -50.0f, 1.0f}, float3{0, -50.0f + 17.67f, 1.0f}},
            Segment{float3{-50, -50, 1.0f}, float3{50, -50.0f, 1.0f}},
            Segment{float3{-50, -50, 1.0f}, float3{-50, 50.0f, 1.0f}},
            Segment{float3{50, -50, 1.0f}, float3{50, 50.0f, 1.0f}},
        });

    //添加质点
    Entity massPoints[5][5];
    constexpr int length = 4;
    for (int i = 0, y = -8; i < 5; i++, y += length)
        for (int j = 0, x = -8; j < 5; j++, x += length)
        {
            massPoints[i][j] = PhysicsSystem::AddMassPoint(
                float3{static_cast<float>(x), static_cast<float>(y), 1}, 0.01f
            );
        }
    //添加弹簧
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
        {
            //维持内部结构
            if (x + 1 < 5)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y][x + 1], 1000);
            if (y + 1 < 5)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y + 1][x], 1000);
            if (x + 1 < 5 && y + 1 < 5) //右下
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y + 1][x + 1], 1000);
            if (x + 1 < 5 && y - 1 >= 0) //右上
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y - 1][x + 1], 1000);
            //维持整体横向结构
            if (x == 0)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y][4], 1000);
            if (y == 0)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[4][x], 1000);
            //维持整体斜向结构
            if (x == 0)
            {
                if (y != 0) //右上
                    PhysicsSystem::AddSpring(massPoints[y][x], massPoints[0][y], 1000);
                if (y != 4) //右下
                    PhysicsSystem::AddSpring(massPoints[y][x], massPoints[4][4 - y], 1000);
            }
            if (x == 4)
            {
                if (y != 0 && y != 4) //左上
                    PhysicsSystem::AddSpring(massPoints[y][x], massPoints[0][4 - y], 1000);
                if (y != 4 && y != 0) //左下
                    PhysicsSystem::AddSpring(massPoints[y][x], massPoints[4][y], 1000);
            }
            //辅助弹簧（移除后仍然能维持结构，但加上效果更真实）
            if (x + 2 < 5)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y][x + 2], 1000);
            if (y + 2 < 5)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y + 2][x], 1000);
        }
}