#pragma once
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"
#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Resource/GLBuffer.h"
#include "LightMath/Runtime/Matrix.hpp"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{




    class MeshBase
    {
    public:
        virtual ~MeshBase() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MeshBase* lastMesh) = 0;
        virtual const GLMeshLayout* GetGLMeshLayout() const = 0;
        /**
         * GLIndexCount等于索引数量，但并非总是有效。因为它是专供图形接口使用的索引数量，因此只有在绑定资源到管线时才被计算。
         * @return 
         */
        virtual uint32_t GetGLIndexCount() const = 0;
    };

    class ShaderBase
    {
    public:
        virtual ~ShaderBase() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderBase* lastShader) = 0;
        virtual const GLPipelineLayout& GetGLPipelineLayout() const = 0;
        virtual const std::vector<GLDescriptorBinding>& GetDescriptorBinding() const = 0;
        virtual const std::vector<VkPushConstantRange>& GetPushConstantBinding() const = 0;
    };

    class MaterialBase
    {
    public:
        virtual ~MaterialBase() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialBase* lastMaterial) = 0;
        virtual const ShaderBase* GetShader() const = 0;
    };

    class TextureBase
    {
    public:
        virtual ~TextureBase() = default;
        virtual const GLImageView& GetGLImageView() const = 0;
        virtual const GLImageSampler& GetGLImageSampler() const = 0;
    };

    class RenderTargetBase
    {
    public:
        virtual ~RenderTargetBase() = default;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual VkSampleCountFlagBits GetSampleCount() const = 0;
        virtual const VkImage& GetVKColorImage() const = 0;
        virtual const VkImage* GetVkDepthStencilImage() const = 0;
        virtual const VkImage* GetVkColorResolveImage() const = 0;
        virtual const GLImageView& GetGLColorImageView() const = 0;
        virtual const GLImageView* GetGLDepthStencilImageView() const = 0;
        virtual const GLImageView* GetGLColorResolveImageView() const = 0;
        float GetAspectRatio() const { return static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()); }
    };
}
