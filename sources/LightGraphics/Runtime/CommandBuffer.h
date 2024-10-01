#pragma once
#include <stack>

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
        CommandBuffer();
        CommandBuffer(const CommandBuffer&) = delete;

        GLCommandBuffer& GetGLCommandBuffer();

        void BeginRecording();
        void EndRecording();

        void BeginRendering(const RenderTexture* renderTarget, bool clearColor = false) const;
        void EndRendering() const;

        void SetViewport(const rect& viewport) const;
        void PushConstant(const Shader& shader, int slotIndex, void* data) const;
        void Draw(const MeshBase& mesh, const Material& material);

    private:
        inline static std::stack<CommandBuffer*> commandBufferPool = {};

        GLCommandBuffer glCommandBuffer;
        const MeshBase* lastMesh;
        const Material* lastMaterial;
        const Shader* lastShader;
    };
}
