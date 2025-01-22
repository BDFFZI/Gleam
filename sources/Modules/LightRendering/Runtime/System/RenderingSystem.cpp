#include "RenderingSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightMath/Runtime/MatrixMath.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/LinesMesh.h"
#include "LightRendering/Runtime/Component/PointsMesh.h"
#include "LightRendering/Runtime/Resource/CommandBufferPool.h"

namespace Light
{
    CameraInfo::CameraInfo(Transform& transform, Camera& camera)
    {
        float4x4 matrixV = inverse(float4x4::TRS(transform.localPosition, transform.localRotation, transform.localScale));
        float4x4 matrixP = camera.orthographic
                               ? float4x4::Ortho(camera.halfHeight * camera.aspect, camera.halfHeight, camera.nearClipPlane, camera.farClipPlane)
                               : float4x4::Perspective(camera.fieldOfView, camera.aspect, camera.nearClipPlane, camera.farClipPlane);
        matrixVP = matrixP * matrixV;
        this->camera = &camera;
    }
    bool CameraInfo::operator<(const CameraInfo& other) const
    {
        return other.camera->depth > other.camera->depth;
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
        vertexColorGSCodeLayout = std::make_unique<GSCodeLayout>("Resources/LightRenderingRuntime/VertexColor.hlsl");
        pointGSInoutLayout = std::make_unique<GSInoutLayout>(
            RenderingConfig::PointMeshLayout, RenderingConfig::ColorFormat, RenderingConfig::DepthStencilFormat);
        lineGSInoutLayout = std::make_unique<GSInoutLayout>(
            RenderingConfig::LineMeshLayout, RenderingConfig::ColorFormat, RenderingConfig::DepthStencilFormat);
        defaultPointShader = std::make_unique<GShader>(*vertexColorGSCodeLayout, *pointGSInoutLayout);
        defaultLineShader = std::make_unique<GShader>(*vertexColorGSCodeLayout, *lineGSInoutLayout);
        defaultPointMaterial = std::make_unique<GMaterial>(*defaultPointShader);
        defaultLineMaterial = std::make_unique<GMaterial>(*defaultLineShader);

        SystemGroup::Start();
    }
    void RenderingSystem::Stop()
    {
        SystemGroup::Stop();

        vertexColorGSCodeLayout.reset();
        pointGSInoutLayout.reset();
        lineGSInoutLayout.reset();
        defaultPointShader.reset();
        defaultLineShader.reset();
        defaultPointMaterial.reset();
        defaultLineMaterial.reset();

        CommandBufferPool::Clear();
    }


    void BuildMesh()
    {
        View<PointsRenderer>::Each([](auto& pointsRenderer)
        {
            if (pointsRenderer.mesh == nullptr)
                pointsRenderer.mesh = std::make_unique<Mesh>(true);

            Mesh* pointMesh = pointsRenderer.mesh.get();
            std::vector<Vertex>& pointVertices = pointMesh->GetVertices();
            std::vector<uint32_t>& pointIndices = pointMesh->GetIndices();

            pointVertices.clear();
            pointIndices.clear();
            int pointIndex = 0;
            for (auto point : pointsRenderer.points)
            {
                pointVertices.emplace_back(point.position, 1.0f);
                pointIndices.emplace_back(pointIndex++);
            }

            pointMesh->SetDirty();
        });
        View<LinesRenderer>::Each([](auto& linesRenderer)
        {
            if (linesRenderer.mesh == nullptr)
                linesRenderer.mesh = std::make_unique<Mesh>(true);

            Mesh* lineMesh = linesRenderer.mesh.get();
            std::vector<Vertex>& lineVertices = lineMesh->GetVertices();
            std::vector<uint32_t>& lineIndices = lineMesh->GetIndices();

            lineVertices.clear();
            lineIndices.clear();
            int lineIndex = 0;
            for (auto line : linesRenderer.lines)
            {
                lineVertices.emplace_back(line.positionA, 1.0f);
                lineIndices.emplace_back(lineIndex++);
                lineVertices.emplace_back(line.positionB, 1.0f);
                lineIndices.emplace_back(lineIndex++);
            }

            lineMesh->SetDirty();
        });
    }

    void RenderingSystem::Update()
    {
        BuildMesh();
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
        CommandBuffer& commandBuffer = CommandBufferPool::Apply();
        for (const auto& cameraInfo : cameraInfos)
        {
            commandBuffer.SetRenderTarget(*cameraInfo.camera->renderTarget);
            commandBuffer.ClearRenderTarget(cameraInfo.camera->background);
            commandBuffer.SetViewProjectionMatrices(cameraInfo.matrixVP);
            for (const auto& rendererInfo : rendererInfos)
            {
                commandBuffer.Draw(*rendererInfo.mesh, rendererInfo.matrixM, *rendererInfo.material);
            }
        }
        CommandBufferPool::Release(commandBuffer);

        SystemGroup::Update();
    }
}