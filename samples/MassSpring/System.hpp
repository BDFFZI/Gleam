#pragma once

#include <imgui.h>

#include "LightECS/Runtime/System.hpp"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"

namespace Light
{
    struct BeginPaintSystem : System<MinSystemOrder, MaxSystemOrder>
    {
        inline static GLCommandBuffer* presentCommandBuffer = nullptr;

        static void Update()
        {
            presentCommandBuffer = &Graphics::BeginPresent();
            UI::BeginFrame();
        }
    };

    struct EndPaintSystem : System<BeginPaintSystem, MaxSystemOrder>
    {
        static void Update()
        {
            UI::EndFrame(*BeginPaintSystem::presentCommandBuffer);
            Graphics::EndPresent(*BeginPaintSystem::presentCommandBuffer);
            Graphics::WaitPresent();
        }
    };

    struct RenderingSystem : System<BeginPaintSystem, EndPaintSystem>
    {
        static void Update()
        {
            CommandBuffer& commandBuffer = Graphics::ApplyCommandBuffer();

            commandBuffer.BeginRecording();
            commandBuffer.ClearRenderTexture(float4(0, 0, 1, 1));
            commandBuffer.EndRecording();
            BeginPaintSystem::presentCommandBuffer->ExecuteSubCommands(commandBuffer.GetGLCommandBuffer());

            Graphics::ReleaseCommandBuffer(commandBuffer);

            // View<Line>::Each([](Line& line)
            // {
            // });
            //
            // std::vector<Vertex>& vector = pointMesh.GetVertices();
            // std::vector<uint32_t>& indices = pointMesh.GetIndices();
            // vector.clear();
            // indices.clear();
            // int index = 0;
            // View<Point, Transform>::Each([&index,&vector,&indices](Point& point, Transform& transform)
            // {
            //     vector.emplace_back(transform.position, point.color);
            //     indices.emplace_back(index++);
            // });
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
        inline static GLMeshLayout lineMeshLayout = {
            vertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false}
        };
        inline static GLMeshLayout pointMeshLayout = {
            vertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false}
        };

        inline static MeshT<Vertex> lineMesh = {&lineMeshLayout, true};
        inline static MeshT<Vertex> pointMesh = {&pointMeshLayout, true};
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
