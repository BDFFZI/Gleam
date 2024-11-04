#pragma once
#include <stack>

#include "Material.h"
#include "Mesh.h"
#include "RenderTexture.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"

namespace Light
{
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(const CommandBuffer&) = delete;

        GLCommandBuffer& GetGLCommandBuffer();

        void BeginRecording();
        void EndRecording();

        void BeginRendering(const RenderTexture* renderTarget, bool clearColor = false) const;
        void EndRendering() const;

        void SetViewport(float x, float y, float width, float height) const;
        void PushConstant(const Shader& shader, int slotIndex, void* data) const;
        void Draw(const MeshBase& mesh, const Material& material);
        void ClearRenderTexture(float4 color = 0, float depth = 0);

    private:
        inline static std::stack<CommandBuffer*> commandBufferPool = {};

        GLCommandBuffer glCommandBuffer = GLCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY);
        const MeshBase* lastMesh = nullptr;
        const Material* lastMaterial = nullptr;
        const Shader* lastShader = nullptr;
    };
}
