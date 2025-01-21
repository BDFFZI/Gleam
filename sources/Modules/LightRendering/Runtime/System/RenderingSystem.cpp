#include "RenderingSystem.h"

#include "LightECS/Runtime/View.hpp"
#include "LightMath/Runtime/MatrixMath.hpp"
#include "LightRendering/Runtime/Component/Camera.hpp"
#include "LightRendering/Runtime/Component/LinesMesh.hpp"
#include "LightRendering/Runtime/Component/PointsMesh.hpp"

namespace Light
{
    CameraInfo::CameraInfo(Transform& transform, Camera& camera)
    {
        float4x4 matrixV = inverse(float4x4::TRS(transform.localPosition, transform.localRotation, transform.localScale));
        float4x4 matrixP = camera.orthographic
                               ? float4x4::Ortho(camera.halfHeight * camera.aspect, camera.halfHeight, camera.nearClipPlane, camera.farClipPlane)
                               : float4x4::Perspective(camera.fieldOfView, camera.aspect, camera.nearClipPlane, camera.farClipPlane);
        matrixVP = matrixP * matrixV;
        depth = camera.depth;
        renderTarget = camera.renderTarget;
    }
    RendererInfo::RendererInfo(Transform& transform, Renderer& renderer)
    {
        matrixM = float4x4::TRS(transform.localPosition, transform.localRotation, transform.localScale);
        renderQueue = renderer.material->GetRenderQueue();
        material = renderer.material;
        mesh = renderer.mesh;
    }

    void RenderingSystem::Start()
    {
        defaultPointShader = std::make_unique<GShader>("Assets/VertexColor.hlsl", GraphicsPreset::DefaultStateLayout, pointMeshLayout);
        defaultLineShader = std::make_unique<GShader>("Assets/VertexColor.hlsl", GraphicsPreset::DefaultStateLayout, lineMeshLayout);
        defaultPointMaterial = std::make_unique<GMaterial>(*defaultPointShader);
        defaultLineMaterial = std::make_unique<GMaterial>(*defaultLineShader);

        SystemGroup::Start();
    }
    void RenderingSystem::Stop()
    {
        SystemGroup::Stop();
    }


    void BuildMesh()
    {
        View<PointsRenderer>::Each([this](auto& pointsRenderer)
        {
            if (pointsRenderer.mesh == nullptr)
                pointsRenderer.mesh = std::make_unique<Mesh>(true);
            Mesh* pointMesh = pointsRenderer.mesh.get();
            std::vector<Vertex>& pointVertices = pointMesh->GetVertices();
            std::vector<uint32_t>& pointIndices = pointMesh->GetIndices();
            pointVertices.clear();
            pointIndices.clear();
            int pointIndex = 0;
            View<Point, Renderer>::Each([&pointIndex,&pointVertices,&pointIndices](Point& point, Renderer& renderer)
            {
                pointVertices.emplace_back(point.position, renderer.color);
                pointIndices.emplace_back(pointIndex++);
            });
            pointMesh->SetDirty();
        });
        View<LinesRenderer>::Each([this](auto& linesRenderer)
        {
            cameraInfos.emplace(transform, camera);
        });
    }

    void RenderingSystem::Update()
    {
        //统计渲染对象
        cameraInfos.clear();
        View<Transform, Camera>::Each([this](auto& transform, auto& camera)
        {
            cameraInfos.emplace(transform, camera);
        });
        rendererInfos.clear();
        View<Transform, Renderer>::Each([this](auto& transform, auto& renderer)
        {
            rendererInfos.emplace(transform, renderer);
        });
        //渲染
        GCommandBuffer& commandBuffer = PresentationSystem->GetCommandBuffer();
        for (const auto& cameraInfo : cameraInfos)
        {
            commandBuffer.SetRenderTarget(*cameraInfo.renderTarget);
            commandBuffer.SetViewProjectionMatrices(cameraInfo.matrixVP);
            for (const auto& rendererInfo : rendererInfos)
            {
                commandBuffer.Draw(*rendererInfo.mesh, rendererInfo.matrixM, *rendererInfo.material);
            }
        }

        SystemGroup::Update();
    }
}