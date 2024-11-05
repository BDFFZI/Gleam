#pragma once
#include "Mesh.h"

namespace Light
{
    struct LineVertex
    {
        float3 position;
        float4 color;
    };

    inline GLMeshLayout lineMeshLayout = {
        sizeof(LineVertex), {
            GLVertexAttribute{offsetof(LineVertex, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{offsetof(LineVertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
        },
        VK_PRIMITIVE_TOPOLOGY_LINE_LIST
    };
    template <>
    inline const GLMeshLayout& MeshTemplate<LineVertex>::GetMeshLayout()
    {
        return lineMeshLayout;
    }
    
    class LineMesh : public MeshTemplate<LineVertex>
    {
    };
}
