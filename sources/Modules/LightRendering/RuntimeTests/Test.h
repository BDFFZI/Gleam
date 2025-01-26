#pragma once
#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/System/SimulationSystem.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightRendering/Runtime/Component/Archetype.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/LinesMesh.h"
#include "LightRendering/Runtime/Component/PointsMesh.h"

using namespace Light;  // NOLINT(clang-diagnostic-header-hygiene)

inline int main()
{
    //创建相机
    Camera camera;
    camera.orthographic = true;
    camera.background = float4{0.2, 0.2, 0.5, 1};
    Entity cameraEntity = Awake->AddEntity(CameraArchetype);
    World::SetComponents(cameraEntity, camera);
    //创建点渲染器
    std::vector points = {
        Point{{0, 1, 1}},
        Point{{0, 0, 1}},
        Point{{2, 0, 1}}
    };
    Entity pointsRendererEntity = Awake->AddEntity(PointsArchetype);
    World::GetComponent<PointsMesh>(pointsRendererEntity).points = points;
    //创建线渲染器
    std::vector lines = {
        Segment{{0, 0, 1}, {0, 1, 1}},
        Segment{{0, 0, 1}, {2, 0, 1}},
        Segment{{0, 1, 1}, {2, 0, 1}},
    };
    Entity linesRendererEntity = Awake->AddEntity(LinesArchetype);
    World::GetComponent<LinesMesh>(linesRendererEntity).lines = lines;


    Engine::Start();
    return 0;
}