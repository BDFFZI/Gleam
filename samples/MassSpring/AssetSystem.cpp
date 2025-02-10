#include "AssetSystem.h"
#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/World.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"

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
    const float bevelLength = static_cast<float>(sqrt(pow(length, 2) + pow(length, 2)));
    for (int i = 0, y = -8; i < 5; i++, y += length)
        for (int j = 0, x = -8; j < 5; j++, x += length)
        {
            const Entity entity = World::AddEntity(MassPointArchetype);
            World::SetComponents(entity, Point{float3{static_cast<float>(x), static_cast<float>(y), 1}});
            massPoints[i][j] = entity;
        }
    //添加弹簧
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
        {
            if (x + 1 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{massPoints[y][x], massPoints[y][x + 1], length});
            }
            if (y + 1 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{massPoints[y][x], massPoints[y + 1][x], length});
            }
            if (x + 1 < 5 && y + 1 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{massPoints[y][x], massPoints[y + 1][x + 1], bevelLength});
            }
            if (x + 1 < 5 && y - 1 >= 0)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{massPoints[y][x], massPoints[y - 1][x + 1], bevelLength});
            }
            if (x + 2 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{massPoints[y][x], massPoints[y][x + 2], length * 2});
            }
            if (y + 2 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{massPoints[y][x], massPoints[y + 2][x], length * 2});
            }
        }
}