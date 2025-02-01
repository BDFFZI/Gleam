#include "CameraSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"

namespace Light
{
    void CameraSystem::Update()
    {
        View<LocalToWorld, Camera, CameraTransform>::Each(
            [](LocalToWorld& localToWorld, Camera& camera, CameraTransform& cameraTransform)
            {
                float aspect = camera.renderTarget.value_or(RenderingSystem->GetDefaultRenderTarget())->GetAspect();
                cameraTransform.worldToView = inverse(localToWorld.value);
                cameraTransform.viewToClip =
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
                cameraTransform.worldToClip = mul(cameraTransform.viewToClip, cameraTransform.worldToView);
            }
        );
    }
}