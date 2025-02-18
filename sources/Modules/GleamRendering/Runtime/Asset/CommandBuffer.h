#pragma once
#include "GleamGraphics/Runtime/GCommandBuffer.h"
#include "GleamRendering/Runtime/RenderingConfig.h"

namespace Gleam
{
    class CommandBuffer : public GCommandBuffer
    {
    public:
        void SetWorldInfo(const WorldInfo& worldInfo) const;
        void SetObjectInfo(const ObjectInfo& objectInfo);

        void Draw(GMesh& mesh, GMaterial& material, uint32_t instanceCount = 1);
        void Blit(GTexture* source, GRenderTarget* destination);

    private:
        GBuffer worldInfoBuffer = GBuffer{sizeof(WorldInfo)};
        ObjectInfo objectInfoBuffer;
    };
}