#include "CameraSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "LightRendering/Runtime/Rendering.h"

namespace Light
{
    void CameraSystem::Update()
    {
        View<LocalToWorld, Camera, WorldToClip>::Each(
            [](LocalToWorld& localToWorld, Camera& camera, WorldToClip& worldToClip)
            {
                float aspect = camera.renderTarget.value_or(RenderingSystem->GetDefaultRenderTarget())->GetAspect();
                worldToClip.worldToView = inverse(localToWorld.value);
                worldToClip.viewToClip =
                    camera.orthographic
                        ? float4x4::Ortho(
                            camera.halfHeight * aspect,
                            camera.halfHeight,
                            camera.nearClipPlane,
                            camera.farClipPlane)
                        : float4x4::Perspective(
                            camera.fieldOfView,
                            aspect,
                            camera.nearClipPlane,
                            camera.farClipPlane);
                worldToClip.value = mul(worldToClip.viewToClip, worldToClip.worldToView);
            }
        );

        View<Camera, WorldToClip, ScreenToWorld>::Each(
            [](Camera& camera, WorldToClip& worldToClip, ScreenToWorld& screenToWorld)
            {
                GRenderTarget* renderTarget = camera.renderTarget.value_or(RenderingSystem->GetDefaultRenderTarget());
                screenToWorld.screenToClip = float4x4{
                    2 / static_cast<float>(renderTarget->width), 0, 0, -1.0f,
                    0, -2 / static_cast<float>(renderTarget->height), 0, 1.0f,
                    0, 0, 1, 0,
                    0, 0, 0, 1,
                };
                screenToWorld.value = mul(inverse(worldToClip.value), screenToWorld.screenToClip);
            }
        );
    }
}