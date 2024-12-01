#include "RenderingSystem.h"
#include "LogicSystem.h"
#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightWindow/Runtime/Input.h"

namespace Light
{
    float2 Rendering::ScreenToWorldPoint(const float2& positionSS)
    {
        float2 positionUV = {
            positionSS.x / static_cast<float>(Graphics::GetPresentRenderTarget().width),
            positionSS.y / static_cast<float>(Graphics::GetPresentRenderTarget().height)
        };
        float2 viewSizeWS = {
            Graphics::GetPresentRenderTarget().GetAspectRatio() * GetOrthoSize(),
            GetOrthoSize()
        };
        float2 positionWS = {
            (positionUV.x - 0.5f) * viewSizeWS.x,
            (positionUV.y - 0.5f) * viewSizeWS.y,
        };
        return positionWS;
    }
    void RenderingSystem::Start()
    {
        pointMesh = std::make_unique<MeshT<Vertex>>(&pointMeshLayout, true);
        lineMesh = std::make_unique<MeshT<Vertex>>(&lineMeshLayout, true);
        pointShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, pointMeshLayout);
        lineShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, lineMeshLayout);
        pointMaterial = std::make_unique<Material>(*pointShader);
        lineMaterial = std::make_unique<Material>(*lineShader);
    }
    void RenderingSystem::Stop()
    {
        pointMaterial.reset();
        lineMaterial.reset();
        pointShader.reset();
        lineShader.reset();
        pointMesh.reset();
        lineMesh.reset();
    }
    void RenderingSystem::Update()
    {
        DrawObject();
    }

    void RenderingSystem::DrawObject()
    {
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

        std::vector<Vertex>& lineVertices = lineMesh->GetVertices();
        std::vector<uint32_t>& lineIndices = lineMesh->GetIndices();
        lineVertices.clear();
        lineIndices.clear();
        int lineIndex = 0;
        View<Line, Renderer>::Each([&lineIndex,&lineVertices,&lineIndices](Line& line, Renderer& renderer)
        {
            lineVertices.emplace_back(line.positionA, renderer.color);
            lineIndices.emplace_back(lineIndex++);
            lineVertices.emplace_back(line.positionB, renderer.color);
            lineIndices.emplace_back(lineIndex++);
        });
        lineMesh->SetDirty();


        auto& commandBuffer = Presentation::GetCommandBuffer();
        commandBuffer.BeginRendering(Graphics::GetPresentRenderTarget(), true);
        {
            commandBuffer.SetViewProjectionMatrices(
                float4x4::Identity(),
                float4x4::Ortho(
                    Graphics::GetPresentRenderTarget().GetAspectRatio() * Rendering::GetOrthoHalfSize(),
                    Rendering::GetOrthoHalfSize(), 0, 1)
            );
            if (!lineMesh->GetIndices().empty())
                commandBuffer.Draw(*lineMesh, *lineMaterial);
            if (!pointMesh->GetIndices().empty())
                commandBuffer.Draw(*pointMesh, *pointMaterial);
        }
        commandBuffer.EndRendering();
    }
}
