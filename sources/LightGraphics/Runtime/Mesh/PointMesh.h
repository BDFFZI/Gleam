#pragma once
#include "Mesh.h"

namespace Light
{
    struct PointVertex
    {
        float3 position;
        float4 color;
    };

    inline GLMeshLayout pointMeshLayout = {
        sizeof(PointVertex), {
            GLVertexAttribute{offsetof(PointVertex, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{offsetof(PointVertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
        },
        VK_PRIMITIVE_TOPOLOGY_POINT_LIST
    };
    template <>
    inline const GLMeshLayout& MeshTemplate<PointVertex>::GetMeshLayout()
    {
        return pointMeshLayout;
    }
    
    class PointMesh : public MeshTemplate<PointVertex>
    {
    };
}
