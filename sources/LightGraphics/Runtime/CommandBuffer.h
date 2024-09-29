#pragma once
#include "Material.h"
#include "Mesh.h"
#include "RenderTexture.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"
#include "LightMath/Runtime/Graphics.h"

namespace LightRuntime
{
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(const CommandBuffer&) = delete;

        GLCommandBuffer& GetGLCommandBuffer();

        void BeginRecording();
        void EndRecording() const;

        void BeginRendering(const RenderTexture* renderTarget, bool clearColor = false) const;
        void EndRendering() const;

        void SetViewport(const rect& viewport) const;
        void Draw(MeshBase& mesh, const Material& material) const;

    private:
        GLCommandBuffer glCommandBuffer;
    };
}
