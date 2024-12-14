#include "RenderingSystem.h"
#include "LogicSystem.h"
#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightGraphics/Runtime/SwapChain.h"
#include "LightWindow/Runtime/Input.h"

namespace Light
{
    float GetAspectRatio(RenderTargetAsset& renderTarget)
    {
        return static_cast<float>(renderTarget.width) / static_cast<float>(renderTarget.height);
    }

    float2 Rendering::ScreenToWorldPoint(const float2& positionSS)
    {
        float2 positionUV = {
            positionSS.x / static_cast<float>(SwapChain::GetPresentRenderTarget().width),
            positionSS.y / static_cast<float>(SwapChain::GetPresentRenderTarget().height)
        };
        float2 viewSizeWS = {
            GetAspectRatio(SwapChain::GetPresentRenderTarget()) * GetOrthoSize(),
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
        pointMesh = std::make_unique<MeshT<Vertex>>(true);
        lineMesh = std::make_unique<MeshT<Vertex>>(true);
        pointShader = std::make_unique<Shader>("Assets/VertexColor.hlsl", GraphicsPreset::DefaultStateLayout, pointMeshLayout);
        lineShader = std::make_unique<Shader>("Assets/VertexColor.hlsl", GraphicsPreset::DefaultStateLayout, lineMeshLayout);
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
        commandBuffer.BeginRendering(SwapChain::GetPresentRenderTarget(), true);
        {
            commandBuffer.SetViewProjectionMatrices(
                float4x4::Identity(),
                float4x4::Ortho(
                    GetAspectRatio(SwapChain::GetPresentRenderTarget()) * Rendering::GetOrthoHalfSize(),
                    Rendering::GetOrthoHalfSize(), 0, 1)
            );
            if (!lineMesh->GetIndices().empty())
                commandBuffer.Draw(*lineMesh, *lineMaterial, float4x4::Identity());
            if (!pointMesh->GetIndices().empty())
                commandBuffer.Draw(*pointMesh, *pointMaterial, float4x4::Identity());
        }
        commandBuffer.EndRendering();
    }
}
