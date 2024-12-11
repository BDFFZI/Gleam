#pragma once
#include "PresentationSystem.h"
#include "LightECS/Runtime/_Template.hpp"
#include "LightGraphics/Runtime/Material.h"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightGraphics/Runtime/Shader.h"

namespace Light
{
    class Rendering
    {
    public:
        consteval static float GetOrthoSize() { return 100.0f; }
        consteval static float GetOrthoHalfSize() { return GetOrthoSize() / 2; }

        static float2 ScreenToWorldPoint(const float2& positionSS);
    };

    struct RenderingSystem : SystemT<PresentationSystemGroup>
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
        inline static std::unique_ptr<ShaderT> pointShader = nullptr;
        inline static std::unique_ptr<ShaderT> lineShader = nullptr;
        inline static std::unique_ptr<MaterialT> pointMaterial = nullptr;
        inline static std::unique_ptr<MaterialT> lineMaterial = nullptr;

        static void DrawObject();
    };
}
