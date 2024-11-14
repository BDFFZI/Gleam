#pragma once
#include <limits>

#include "Component.hpp"
#include "LightECS/Runtime/View.hpp"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightGraphics/Runtime/Mesh.h"

namespace Light
{


    class BeginPresentSystem : public SystemT<SystemMinOrder, SystemMaxOrder>
    {
    public:
        inline static GLCommandBuffer* presentCommandBuffer = nullptr;

        static void Update()
        {
            presentCommandBuffer = &Graphics::BeginPresent();
        }
    };

    class EndPresentSystem : public SystemT<BeginPresentSystem::Order, SystemMaxOrder>
    {
    public:
        static void Update()
        {
            Graphics::EndPresent(*BeginPresentSystem::presentCommandBuffer);
        }
    };

    class RenderingSystem : public SystemT<SystemMinOrder, SystemMaxOrder>
    {
    public:
        static void Update()
        {
            View<Line> lineView = {*World};
            View<Point, Transform> pointView = {*World};

            lineView.Each([](Line& line)
            {
            });

            std::vector<Vertex>& vector = pointMesh.GetVertices();
            std::vector<uint32_t>& indices = pointMesh.GetIndices();
            vector.clear();
            indices.clear();
            int index = 0;
            pointView.Each([&index,&vector,&indices](Point& point, Transform& transform)
            {
                vector.emplace_back(transform.position, point.color);
                indices.emplace_back(index++);
            });
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
}
