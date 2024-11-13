#pragma once
#include <limits>

#include "Component.hpp"
#include "LightECS/Runtime/View.hpp"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightGraphics/Runtime/Mesh.h"

using namespace Light;

constexpr float SystemMinOrder = std::numeric_limits<float>::min();
constexpr float SystemMaxOrder = std::numeric_limits<float>::max();

class System
{
public:
    EntitySet* entitySet;
    
    virtual ~System() = default;

    virtual void Start()
    {
    }
    virtual void Stop()
    {
    }
    virtual void Update()
    {
    }
};

template <float MinOrder, float MaxOrder>
class SystemT : System
{
public:
    static constexpr float Order = (MinOrder + MaxOrder) / 2;
};

class BeginPresentSystem : public SystemT<SystemMinOrder, SystemMaxOrder>
{
    BeginPresentSystem(): System(SystemMinOrder, SystemMaxOrder)
    {
    }

    void Update() override
    {
        CommandBuffer& commandBuffer = Graphics::GetCommandBuffer();
        commandBuffer.BeginRecording();
        commandBuffer.BeginRendering(Graphics::GetPresentRenderTexture());
    }
};

class EndPresentSystem: public SystemT<BeginPresentSystem::Order, SystemMaxOrder>
{
public:
    EndPresentSystem(BeginPresentSystem& beginPresentSystem):
        System(BeginPresentSystem::Order, SystemMaxOrder),
        beginPresentSystem(&beginPresentSystem)
    {
    }

    void Update(EntitySet& entitySet) override
    {
    }

private:
    BeginPresentSystem* beginPresentSystem;
};

class RenderingSystem : public System<SystemMinOrder, SystemMaxOrder>
{
    struct Vertex
    {
        float2 position;
        float4 color;
    };
    GLVertexInput vertexInput = {
        sizeof(Vertex), {
            GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{4,offsetof(Vertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
        }
    };
    GLMeshLayout lineMeshLayout = {
        vertexInput,
        GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false}
    };
    GLMeshLayout pointMeshLayout = {
        vertexInput,
        GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false}
    };

    Light::MeshT<Vertex> lineMesh = {&lineMeshLayout, true};
    Light::MeshT<Vertex> pointMesh = {&pointMeshLayout, true};

    void Update(EntitySet& entitySet) override
    {
        View<Line> lineView = {entitySet};
        View<Point, Transform> pointView = {entitySet};

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
};

class PhysicsSystem : public System<RenderingSystem::Order, SystemMaxOrder>
{
};
