#pragma once
#include "GleamECS/Runtime/View/View.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamEngine/Runtime/Entity/Transform.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamRendering/Runtime/Entity/Camera.h"
#include "GleamRendering/Runtime/Entity/LinesMesh.h"
#include "GleamRendering/Runtime/Entity/PointsMesh.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

using namespace Gleam;

struct TestCameraData
{
    float4 mousePositionCS;
};

Gleam_MakeTypeWithID(TestCameraData, "")
{
    Gleam_MakeType_AddField(mousePositionCS);
}

Gleam_MakeArchetypeChild(TestCameraArchetype, CameraArchetype, ScreenToClip, TestCameraData)

Gleam_MakeInitEvent()
{
    //创建相机
    Camera camera;
    camera.orthographic = true;
    camera.background = float4{0.2f, 0.2f, 0.5f, 1};
    Entity cameraEntity = World::AddEntity(TestCameraArchetype);
    World::GetEntityAllocator().SetComponents(cameraEntity, camera);
    //创建点渲染器
    std::vector points = {
        Point{{0, 1, 1}},
        Point{{0, 0, 1}},
        Point{{2, 0, 1}}
    };
    Entity pointsRendererEntity = World::AddEntity(PointRendererArchetype);
    World::GetEntityAllocator().GetComponent<PointsMesh>(pointsRendererEntity).points = points;
    //创建线渲染器
    std::vector lines = {
        Segment{{0, 0, 1}, {0, 1, 1}},
        Segment{{0, 0, 1}, {2, 0, 1}},
        Segment{{0, 1, 1}, {2, 0, 1}},
    };
    Entity linesRendererEntity = World::AddEntity(LineRendererArchetype);
    World::GetEntityAllocator().GetComponent<LinesMesh>(linesRendererEntity).lines = lines;
}

Gleam_Main