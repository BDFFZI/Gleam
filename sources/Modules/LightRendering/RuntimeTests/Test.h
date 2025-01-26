#pragma once
#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/System.h"
#include "LightEngine/Runtime/Component.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/LinesMesh.h"
#include "LightRendering/Runtime/Component/PointsMesh.h"

using namespace Light;

Light_MakeArchetype(CameraArchetype, Transform, Camera)
Light_MakeArchetype(PointsArchetype, Transform, PointsMesh, Renderer)
Light_MakeArchetype(LinesArchetype, Transform, LinesMesh, Renderer)

inline int main()
{
    //创建相机
    Camera camera;
    camera.orthographic = true;
    camera.background = float4{0.2, 0.2, 0.2, 1};
    Entity cameraEntity = Awake->AddEntity(CameraArchetype);
    World::SetComponents(cameraEntity, camera);
    //创建点渲染器
    float3 pointMap[5][5];
    std::vector<Point> points;
    constexpr int length = 1;
    for (int i = 0, y = -2; i < 5; i++, y += length)
        for (int j = 0, x = -2; j < 5; j++, x += length)
        {
            pointMap[i][j] = points.emplace_back(
                float3({static_cast<float>(x), static_cast<float>(y)}, 1)
            ).position;
        }
    Entity pointsRendererEntity = Awake->AddEntity(PointsArchetype);
    World::GetComponent<PointsMesh>(pointsRendererEntity).points = points;
    //创建线渲染器
    std::vector<Segment> lines;
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
        {
            if (x + 1 < 5)
                lines.emplace_back(pointMap[y][x], pointMap[y][x + 1]);
            if (y + 1 < 5)
                lines.emplace_back(pointMap[y][x], pointMap[y + 1][x]);
            if (x + 1 < 5 && y + 1 < 5)
                lines.emplace_back(pointMap[y][x], pointMap[y + 1][x + 1]);
            if (x + 1 < 5 && y - 1 >= 0)
                lines.emplace_back(pointMap[y][x], pointMap[y - 1][x + 1]);
            if (x + 2 < 5)
                lines.emplace_back(pointMap[y][x], pointMap[y][x + 2]);
            if (y + 2 < 5)
                lines.emplace_back(pointMap[y][x], pointMap[y + 2][x]);
        }
    Entity linesRendererEntity = Awake->AddEntity(LinesArchetype);
    World::GetComponent<LinesMesh>(linesRendererEntity).lines = lines;


    Engine::Start();
    return 0;
}