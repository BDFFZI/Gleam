#include "CameraSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Rendering.h"

namespace Gleam
{
    void CameraSystem::Update()
    {
        View<Camera, ViewToClip>::Each(
            [](Camera& camera, ViewToClip& worldToClip)
            {
                float aspect = camera.renderTarget.value_or(GlobalRenderingSystem.GetDefaultRenderTarget()).get().GetAspect();
                worldToClip.value =
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
            }
        );

        View<WorldToLocal, ViewToClip, WorldToClip>::Each(
            [](WorldToLocal& worldToLocal, ViewToClip& viewToClip, WorldToClip& worldToClip)
            {
                worldToClip.value = mul(viewToClip.value, worldToLocal.value);
            }
        );

        View<Camera, ScreenToClip>::Each([](Camera& camera, ScreenToClip& screenToClip)
        {
            GRenderTarget& renderTarget = camera.renderTarget.value_or(GlobalRenderingSystem.GetDefaultRenderTarget());
            screenToClip.value = float4x4{
                2 / static_cast<float>(renderTarget.width), 0, 0, -1.0f,
                0, -2 / static_cast<float>(renderTarget.height), 0, 1.0f,
                0, 0, 1, 0,
                0, 0, 0, 1,
            };
        });

        View<ScreenToClip, WorldToClip, ScreenToWorld>::Each(
            [](ScreenToClip& screenToClip, WorldToClip& worldToClip, ScreenToWorld& screenToWorld)
            {
                screenToWorld.value = mul(inverse(worldToClip.value), screenToClip.value);
            }
        );
    }
}