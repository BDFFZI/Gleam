#pragma once
#include "PresentationSystem.h"
#include "LightECS/Runtime/System.hpp"
#include "LightGraphics/Runtime/Material.h"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightGraphics/Runtime/Shader.h"

namespace Light
{
    struct RenderingSystem : System<PresentationSystemGroup>
    {
        static void Start();
        static void Stop();
        static void Update();

    private:
        struct Vertex
        {
            float2 position;
            float4 color;
        };
        inline static GLVertexInput vertexInput = {
            sizeof(Vertex), {
                GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32_SFLOAT},
                GLVertexAttribute{1,offsetof(Vertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
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

        static void DrawObject();
        static void DrawUI();
    };
}
