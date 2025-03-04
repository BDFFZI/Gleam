#include "AssetSystem.h"
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/World.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"
#include "GleamMath/Runtime/Geometry/2D/Rectangle.h"

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
    Entity collider1 = World::AddEntity(RectangleCollider);
    World::SetComponents(collider1, Gleam::Rectangle{float2{-90, -100}, float2{90, -50}});
    Entity collider2 = World::AddEntity(RectangleCollider);
    World::SetComponents(collider2, Gleam::Rectangle{float2{-100, -90}, float2{-50, 90}});
    Entity collider3 = World::AddEntity(RectangleCollider);
    World::SetComponents(collider3, Gleam::Rectangle{float2{50, -90}, float2{100, 90}});
    Entity collider4 = World::AddEntity(RectangleCollider);
    World::SetComponents(collider4, Rectangle::CreateFromCenter(float2{0, 0}, float2(100, 100)), LocalTransform{{0, -50 - 70.71f + 17.67f, 0}, Quaternion::Euler({0, 0, 45})});
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
    Entity particles[5][5];
    constexpr int length = 4;
    for (int i = 0, y = -8; i < 5; i++, y += length)
        for (int j = 0, x = -8; j < 5; j++, x += length)
        {
            particles[i][j] = PhysicsSystem::AddParticle(
                float3{static_cast<float>(x), static_cast<float>(y), 1}, 0.01f
            );
        }
    //添加弹簧
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
        {
            constexpr float elasticity = 0.75f;
            //维持内部结构
            if (x + 1 < 5)
                PhysicsSystem::AddSpring(particles[y][x], particles[y][x + 1], elasticity);
            if (y + 1 < 5)
                PhysicsSystem::AddSpring(particles[y][x], particles[y + 1][x], elasticity);
            if (x + 1 < 5 && y + 1 < 5) //右下
                PhysicsSystem::AddSpring(particles[y][x], particles[y + 1][x + 1], elasticity);
            if (x + 1 < 5 && y - 1 >= 0) //右上
                PhysicsSystem::AddSpring(particles[y][x], particles[y - 1][x + 1], elasticity);
            //维持整体横向结构
            if (x == 0)
                PhysicsSystem::AddSpring(particles[y][x], particles[y][4], elasticity);
            if (y == 0)
                PhysicsSystem::AddSpring(particles[y][x], particles[4][x], elasticity);
            //维持整体斜向结构
            if (x == 0)
            {
                if (y != 0) //右上
                    PhysicsSystem::AddSpring(particles[y][x], particles[0][y], elasticity);
                if (y != 4) //右下
                    PhysicsSystem::AddSpring(particles[y][x], particles[4][4 - y], elasticity);
            }
            if (x == 4)
            {
                if (y != 0 && y != 4) //左上
                    PhysicsSystem::AddSpring(particles[y][x], particles[0][4 - y], elasticity);
                if (y != 4 && y != 0) //左下
                    PhysicsSystem::AddSpring(particles[y][x], particles[4][y], elasticity);
            }
            //辅助弹簧（移除后仍然能维持结构，但加上效果更真实）
            if (x + 2 < 5)
                PhysicsSystem::AddSpring(particles[y][x], particles[y][x + 2], elasticity);
            if (y + 2 < 5)
                PhysicsSystem::AddSpring(particles[y][x], particles[y + 2][x], elasticity);
        }
    centerParticle = particles[2][2];
}