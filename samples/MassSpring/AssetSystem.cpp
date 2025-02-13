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
    //添加质点和弹簧的渲染器
    World::AddEntity(MassPointRendererArchetype);
    World::AddEntity(SpringRendererArchetype);
    //添加质点
    Entity massPoints[5][5];
    constexpr int length = 4;
    for (int i = 0, y = -8; i < 5; i++, y += length)
        for (int j = 0, x = -8; j < 5; j++, x += length)
        {
            massPoints[i][j] = PhysicsSystem::AddMassPoint(
                float3{static_cast<float>(x), static_cast<float>(y), 1}
            );
        }
    //添加弹簧
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
        {
            //内部结构
            if (x + 1 < 5)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y][x + 1]);
            if (y + 1 < 5)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y + 1][x]);
            if (x + 1 < 5 && y + 1 < 5)//右下
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y + 1][x + 1]);
            if (x + 1 < 5 && y - 1 >= 0)//右上
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y - 1][x + 1]);
            //横向结构
            if (x == 0)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y][4]);
            if (y == 0)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[4][x]);
            //斜向结构
            if (x == 0)
            {
                if (y != 0)//右上
                    PhysicsSystem::AddSpring(massPoints[y][x], massPoints[0][y]);
                if (y != 4)//右下
                    PhysicsSystem::AddSpring(massPoints[y][x], massPoints[4][4 - y]);
            }
            if (x == 4)
            {
                if (y != 0 && y != 4)//左上
                    PhysicsSystem::AddSpring(massPoints[y][x], massPoints[0][4 - y]);
                if (y != 4 && y != 0)//左下
                    PhysicsSystem::AddSpring(massPoints[y][x], massPoints[4][y]);
            }
            //辅助弹簧（移除后仍然能维持结构，但加上效果更真实）
            if (x + 2 < 5)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y][x + 2]);
            if (y + 2 < 5)
                PhysicsSystem::AddSpring(massPoints[y][x], massPoints[y + 2][x]);
        }
}