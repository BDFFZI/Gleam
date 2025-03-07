﻿#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMath/Runtime/Random.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Editor/Gizmos.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamRendering/Runtime/Component/Camera.h"
#include "GleamRendering/Runtime/Component/LinesMesh.h"
#include "GleamRendering/Runtime/Component/PointsMesh.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

using namespace Gleam;

struct TestCameraData
{
    float4 mousePositionCS;
};

Gleam_MakeType(TestCameraData, "")
{
    Gleam_MakeType_AddField(mousePositionCS);
}

Gleam_MakeArchetypeChild(TestCameraArchetype, CameraArchetype, ScreenToClip, TestCameraData)

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
        View<ScreenToClip, TestCameraData>::Each([](ScreenToClip& screenToClip, TestCameraData& testCameraData)
        {
            testCameraData.mousePositionCS = mul(screenToClip.value, float4(InputSystem.GetMousePosition(), 0, 1));
        });

        Random::seed = 0;
        for (int i = 0; i < 5; i++)
            Gizmos::DrawWire(Cuboid::Create({Random::OnUnitSphere() * 5}, {Random::OnUnitSphere()}), Random::ColorHSV());
        for (int i = 0; i < 5; i++)
            Gizmos::Draw(Sphere({Random::OnUnitSphere() * 5}, {Random::Value()}), Random::ColorHSV());
        Gizmos::PushLocalToWorld(float4x4::TRS(float3{-1, 0, 0}, float3{-90, 0, 0}, abs(std::sin(TimeSystem.GetTime())) + 1));
        Gizmos::Draw(float3{1, 1, 0}, float4::Red());
        Gizmos::Draw(Segment{{1, 0, 0}, {1, 1, 1}}, float4::Red());
        Gizmos::PopLocalToWorld();
    }
};
Gleam_MakeGlobalSystem(TestSystem)
Gleam_AddSystems(TestSystem)

Gleam_Main