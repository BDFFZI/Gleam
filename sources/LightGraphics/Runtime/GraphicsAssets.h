#pragma once
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"

namespace Light
{
    struct MeshAsset
    {
        GLBuffer* glVertexBuffer;
        GLBuffer* glIndexBuffer;
        uint32_t glIndexCount;

        virtual ~MeshAsset() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MeshAsset* currentMesh);
    };

    struct ShaderAsset
    {
        const std::vector<GLDescriptorBinding>* glDescriptorBindings;
        const std::vector<VkPushConstantRange>* glPushConstantRanges;
        const GLPipelineLayout* glPipelineLayout;
        const GLPipeline* glPipeline;

        virtual ~ShaderAsset() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderAsset* currentShader);
    };

    struct MaterialAsset
    {
        ShaderAsset* shaderAsset;
        std::vector<VkWriteDescriptorSet> glDescriptorSets;
        std::vector<std::vector<std::byte>> glPushConstants;

        virtual ~MaterialAsset() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialAsset* currentMaterial);
    };

    struct TextureAsset
    {
        GLImageView* glImageView;
        GLImageSampler* glImageSampler;
    };

    struct RenderTargetAsset
    {
        /**
         * @brief 渲染后希望的布局（意图，如作为呈现源或着色器纹理）
         *
         * 由于没有使用renderPass，因此布局变换得手动设置。
         * 每次渲染后图片布局默认为“未定义”，需在使用前调整为使用所需的布局。
         * 
         * @return 
         */
        VkImageLayout glTargetLayout;
        uint32_t width, height; //渲染区域
        GLImageView* glColorImageView; //颜色附件
        GLImageView* glDepthStencilImageView; //深度模板附件
        GLImageView* glColorResolveImageView; //颜色重解算附件
    };
}
