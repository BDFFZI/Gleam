#include "GraphicsAssets.h"

namespace Light
{
    void MeshAsset::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MeshAsset* currentMesh)
    {
        glCommandBuffer.BindVertexBuffers(*glVertexBuffer);
        glCommandBuffer.BindIndexBuffer(*glIndexBuffer);
    }


}
