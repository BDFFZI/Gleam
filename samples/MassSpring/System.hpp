#pragma once
#include <limits>

#include "Component.hpp"
#include "LightECS/Runtime/View.hpp"
#include "LightGraphics/Runtime/Mesh.h"

struct Point;
constexpr float SystemMinOrder = std::numeric_limits<float>::min();
constexpr float SystemMaxOrder = std::numeric_limits<float>::max();

template <float MinOrder, float MaxOrder>
class System
{
public:
    static constexpr float Order = (MinOrder + MaxOrder) / 2;
    virtual ~System() = default;
    virtual void Update(EntitySet& entitySet) = 0;
};

class RenderingSystem : public System<SystemMinOrder, SystemMaxOrder>
{
    Light::Mesh lineMesh;
    Light::Mesh pointMesh;
    GLMeshLayout lineMeshLayout = {
        Light::BuiltInGLVertexInput,
        GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false}
    };
    GLMeshLayout pointMeshLayout = {
        Light::BuiltInGLVertexInput,
        GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false}
    };

    void Update(EntitySet& entitySet) override
    {
        View<Line> lineView;
        lineView.Each(entitySet, [](Line& line)
        {
            
        });

        View<Point> pointView;
    }
};

class PhysicsSystem : public System<RenderingSystem::Order, SystemMaxOrder>
{
};
