#include "GleamRendering/RuntimeTests/Tests.h"
#include "GleamMath/Runtime/Random.h"
#include "GleamRendering/Editor/Gizmos.h"

class TestSystem : public System
{
    void Start() override
    {
    }
    void Update() override
    {
        View<ScreenToClip, TestCameraData>::Each([](ScreenToClip& screenToClip, TestCameraData& testCameraData)
        {
            testCameraData.mousePositionCS = mul(screenToClip.value, float4(GlobalInputSystem.GetMousePosition(), 0, 1));
        });

        Random::seed = 0;
        for (int i = 0; i < 5; i++)
            Gizmos::DrawWire(Cuboid::Create({Random::OnUnitSphere() * 5}, {Random::OnUnitSphere()}), Random::ColorHSV());
        for (int i = 0; i < 5; i++)
            Gizmos::Draw(Sphere({Random::OnUnitSphere() * 5}, {Random::Value()}), Random::ColorHSV());
        Gizmos::PushLocalToWorld(float4x4::TRS(float3{-1, 0, 0}, float3{-90, 0, 0}, abs(std::sin(GlobalTimeSystem.GetTime())) + 1));
        Gizmos::Draw(float3{1, 1, 0}, float4::Red());
        Gizmos::Draw(Segment{{1, 0, 0}, {1, 1, 1}}, float4::Red());
        Gizmos::PopLocalToWorld();
    }
};
Gleam_MakeGlobalSystem(TestSystem)
Gleam_AddSystems(GlobalTestSystem)