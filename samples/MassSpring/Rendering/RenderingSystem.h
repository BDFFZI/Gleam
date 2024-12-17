#pragma once
#include "LightECS/Runtime/System.h"
#include "LightGraphics/Runtime/Material.h"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightGraphics/Runtime/Shader.h"
#include "../Public/PresentationSystem.h"

namespace Light
{
    class RenderingSystem : public System
    {
    public:
        RenderingSystem(): System(&PresentationSystem, LeftOrder, MiddleOrder)
        {
        }

        float GetOrthoSize() const { return orthoSize; }
        float GetOrthoHalfSize() const { return orthoSize / 2; }
        float2 ScreenToWorldPoint(const float2& positionSS) const;

    private:
        struct Vertex
        {
            float2 position;
            float4 color;
        };
        GLVertexInput vertexInput = {
            sizeof(Vertex), {
                GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32_SFLOAT},
                GLVertexAttribute{1,offsetof(Vertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
            }
        };
        GLMeshLayout pointMeshLayout = {
            vertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false}
        };
        GLMeshLayout lineMeshLayout = {
            vertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false}
        };

        float orthoSize = 100.0f;
        std::unique_ptr<MeshT<Vertex>> pointMesh = nullptr;
        std::unique_ptr<MeshT<Vertex>> lineMesh = nullptr;
        std::unique_ptr<Shader> pointShader = nullptr;
        std::unique_ptr<Shader> lineShader = nullptr;
        std::unique_ptr<Material> pointMaterial = nullptr;
        std::unique_ptr<Material> lineMaterial = nullptr;

        void DrawObject() const;
        void Start() override;
        void Stop() override;
        void Update() override;
    };
    inline RenderingSystem RenderingSystem = {};
}
