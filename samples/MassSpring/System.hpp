#pragma once

#include <imgui.h>

#include "Component.hpp"
#include "LightECS/Runtime/System.hpp"
#include "LightECS/Runtime/View.hpp"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightGraphics/Runtime/Material.h"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightGraphics/Runtime/Shader.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"

namespace Light
{
    struct BeginPaintSystem : System<MinSystemOrder, MaxSystemOrder>
    {
        inline static GLCommandBuffer* presentCommandBuffer = nullptr;
        inline static CommandBuffer* commandBuffer = nullptr;

        static void Update()
        {
            presentCommandBuffer = &Graphics::BeginPresent();
            commandBuffer = &Graphics::ApplyCommandBuffer();
            commandBuffer->BeginRecording();

            UI::BeginFrame();
        }
    };

    struct EndPaintSystem : System<BeginPaintSystem, MaxSystemOrder>
    {
        static void Update()
        {
            BeginPaintSystem::commandBuffer->BeginRendering(Graphics::GetPresentRenderTexture());
            UI::EndFrame(BeginPaintSystem::commandBuffer->GetGLCommandBuffer());
            BeginPaintSystem::commandBuffer->EndRendering();

            BeginPaintSystem::commandBuffer->EndRecording();
            BeginPaintSystem::presentCommandBuffer->ExecuteSubCommands(BeginPaintSystem::commandBuffer->GetGLCommandBuffer());
            Graphics::EndPresent(*BeginPaintSystem::presentCommandBuffer);

            Graphics::WaitPresent();
            Graphics::ReleaseCommandBuffer(*BeginPaintSystem::commandBuffer);
        }
    };

    struct RenderingSystem : System<BeginPaintSystem, EndPaintSystem>
    {
        static void Start()
        {
            pointMesh = std::make_unique<MeshT<Vertex>>(&pointMeshLayout, true);
            lineMesh = std::make_unique<MeshT<Vertex>>(&lineMeshLayout, true);
            pointShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, pointMeshLayout);
            lineShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, lineMeshLayout);
            pointMaterial = std::make_unique<Material>(pointShader.get());
            lineMaterial = std::make_unique<Material>(lineShader.get());
        }
        static void Stop()
        {
            pointMaterial.reset();
            lineMaterial.reset();
            pointShader.reset();
            lineShader.reset();
            pointMesh.reset();
            lineMesh.reset();
        }

        static void Update()
        {
            View<Line>::Each([](Line& line)
            {
            });

            std::vector<Vertex>& vector = pointMesh->GetVertices();
            std::vector<uint32_t>& indices = pointMesh->GetIndices();
            vector.clear();
            indices.clear();
            int index = 0;
            View<Point, Transform>::Each([&index,&vector,&indices](Point& point, Transform& transform)
            {
                vector.emplace_back(transform.position, point.color);
                indices.emplace_back(index++);
            });


            auto& commandBuffer = *BeginPaintSystem::commandBuffer;
            commandBuffer.BeginRendering(Graphics::GetPresentRenderTexture(), true);
            if (pointMesh->GetIndexCount() != 0)
                commandBuffer.Draw(pointMesh.get(), pointMaterial.get());
            if (lineMesh->GetIndexCount() != 0)
                commandBuffer.Draw(lineMesh.get(), lineMaterial.get());
            commandBuffer.EndRendering();
        }

    private:
        struct Vertex
        {
            float2 position;
            float4 color;
        };
        inline static GLVertexInput vertexInput = {
            sizeof(Vertex), {
                GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{4,offsetof(Vertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
            }
        };
        inline static GLMeshLayout pointMeshLayout = {
            vertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false}
        };
        inline static GLMeshLayout lineMeshLayout = {
            vertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false}
        };

        inline static std::unique_ptr<MeshT<Vertex>> pointMesh = nullptr;
        inline static std::unique_ptr<MeshT<Vertex>> lineMesh = nullptr;
        inline static std::unique_ptr<Shader> pointShader = nullptr;
        inline static std::unique_ptr<Shader> lineShader = nullptr;
        inline static std::unique_ptr<Material> pointMaterial = nullptr;
        inline static std::unique_ptr<Material> lineMaterial = nullptr;
    };

    struct UISystem : System<BeginPaintSystem, EndPaintSystem>
    {
        static void Update()
        {
            ImGui::ShowDemoWindow();
        }
    };

    struct UserInputSystem : System<MinSystemOrder, BeginPaintSystem>
    {
        static void Update()
        {
            if (Input::GetKeyDown(KeyCode::Esc))
            {
                Window::Stop();
            }
        }
    };
}
