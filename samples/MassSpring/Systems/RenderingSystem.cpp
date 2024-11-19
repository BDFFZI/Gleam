#include "RenderingSystem.h"

#include <imgui.h>

#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"
#include "LightGraphics/Runtime/Graphics.h"

namespace Light
{
    void RenderingSystem::Start()
    {
        pointMesh = std::make_unique<MeshT<Vertex>>(&pointMeshLayout, true);
        lineMesh = std::make_unique<MeshT<Vertex>>(&lineMeshLayout, true);
        pointShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, pointMeshLayout);
        lineShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, lineMeshLayout);
        pointMaterial = std::make_unique<Material>(pointShader.get());
        lineMaterial = std::make_unique<Material>(lineShader.get());
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
        DrawUI();
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


        auto& commandBuffer = Presentation::GetCommandBuffer();
        commandBuffer.BeginRendering(Graphics::GetPresentRenderTexture(), true);
        commandBuffer.SetViewProjectionMatrices(float4x4::Identity(), float4x4::Ortho(50, 50, 0, 1));
        if (pointMesh->GetIndexCount() != 0)
            commandBuffer.Draw(pointMesh.get(), pointMaterial.get());
        if (lineMesh->GetIndexCount() != 0)
            commandBuffer.Draw(lineMesh.get(), lineMaterial.get());
        commandBuffer.EndRendering();
    }
    void RenderingSystem::DrawUI()
    {
        ImGui::ShowDemoWindow();
    }
}
