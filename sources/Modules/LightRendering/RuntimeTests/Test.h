#pragma once
#include "LightECS/Runtime/View.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/System/UpdateSystem.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "LightRendering/Runtime/Entity/Archetype.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/LinesMesh.h"
#include "LightRendering/Runtime/Component/PointsMesh.h"
#include "LightWindow/Runtime/Input.h"

using namespace Light; // NOLINT(clang-diagnostic-header-hygiene)

struct TestCameraData
{
    float4 mousePositionWS;
};

Light_MakeType(TestCameraData, "")
{
    Light_MakeType_AddField(mousePositionWS);
}

Light_MakeArchetypeChild(TestCameraArchetype, CameraArchetype, ScreenToWorld, TestCameraData)

class TestSystem : public System
{
    void Update() override
    {
        View<ScreenToWorld, TestCameraData>::Each([](ScreenToWorld& screenToWorld, TestCameraData& testCameraData)
        {
            testCameraData.mousePositionWS = mul(screenToWorld.value, float4(Input->GetMousePosition(), 0, 1));
        });
    }
};
Light_MakeGlobalSystem(TestSystem)

Light_MakeInitEvent()
{
    World::AddSystem(TestSystem);
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