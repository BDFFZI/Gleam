#pragma once
#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamRendering/Runtime/Component/Camera.h"
#include "GleamRendering/Runtime/Component/LinesMesh.h"
#include "GleamRendering/Runtime/Component/PointsMesh.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

using namespace Gleam; // NOLINT(clang-diagnostic-header-hygiene)

struct TestCameraData
{
    float4 mousePositionWS;
};

Gleam_MakeType(TestCameraData, "")
{
    Gleam_MakeType_AddField(mousePositionWS);
}

Gleam_MakeArchetypeChild(TestCameraArchetype, CameraArchetype, ScreenToWorld, TestCameraData)

class TestSystem : public System
{
    void Start() override
    {
        //创建相机
        Camera camera;
        camera.orthographic = true;
        camera.background = float4{0.2f, 0.2f, 0.5f, 1};
        Entity cameraEntity = World::AddEntity(TestCameraArchetype);
        World::SetComponents(cameraEntity, camera);
        //创建点渲染器
        std::vector points = {
            Point{{0, 1, 1}},
            Point{{0, 0, 1}},
            Point{{2, 0, 1}}
        };
        Entity pointsRendererEntity = World::AddEntity(PointRendererArchetype);
        World::GetComponent<PointsMesh>(pointsRendererEntity).points = points;
        //创建线渲染器
        std::vector lines = {
            Segment{{0, 0, 1}, {0, 1, 1}},
            Segment{{0, 0, 1}, {2, 0, 1}},
            Segment{{0, 1, 1}, {2, 0, 1}},
        };
        Entity linesRendererEntity = World::AddEntity(LineRendererArchetype);
        World::GetComponent<LinesMesh>(linesRendererEntity).lines = lines;
    }
    void Update() override
    {
        View<ScreenToWorld, TestCameraData>::Each([](ScreenToWorld& screenToWorld, TestCameraData& testCameraData)
        {
            testCameraData.mousePositionWS = mul(screenToWorld.value, float4(InputSystem->GetMousePosition(), 0, 1));
        });
    }
};
Gleam_MakeSystemInstance(TestSystem)
Gleam_AddSystems(TestSystem)