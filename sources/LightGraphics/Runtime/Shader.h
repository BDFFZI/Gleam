#pragma once
#include <memory>

#include "Mesh.h"
#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Pipeline/GLPipelineLayout.h"
#include "LightGL/Runtime/Pipeline/GLShader.h"
#include "LightGL/Runtime/Resource/GLDescriptorPool.h"
#include "LightMath/Runtime/Matrix.hpp"

namespace Light
{
    struct PushConstant
    {
        alignas(16) float4x4 objectToWorld;
        alignas(16) float4x4 worldToView;
        alignas(16) float4x4 viewToClip;
    };

    class Shader
    {
    public:
        /**
         * @brief 通过文件加载符合规范的着色器
         *
         * 规范要求：
         * 1. 只支持顶点和片段着色器，且名称必须分别为@c VertexShader @c FragmentShader 。
         * 2. 只能使用内置的推送常量类型。
         * 3. 描述符集只能以推送的方式使用。
         * 4. 输出的帧缓冲区格式只能使用交换链中的格式。
         * 5. 默认使用内置的网格布局
         * 
         * @param shaderFile 
         * @param descriptorSetLayout 
         * @param stateLayout
         * @param meshLayout 
         * @return 
         */
        static std::unique_ptr<Shader> CreateFromFile(
            const std::string& shaderFile,
            const std::vector<GLDescriptorBinding>& descriptorSetLayout = {},
            const StateLayout& stateLayout = {},
            const GLMeshLayout& meshLayout = Mesh::GetMeshLayout()
        );

        Shader(
            const std::vector<GLShader>& shaderLayout,
            const std::vector<GLDescriptorBinding>& descriptorBindings, VkDescriptorSetLayoutCreateFlags descriptorFlags,
            const std::vector<VkPushConstantRange>& pushConstantRanges,
            const GLMeshLayout& meshLayout, const StateLayout& stateLayout,
            VkFormat colorFormat, VkFormat depthStencilFormat);
        Shader(const Shader&) = delete;

        const GLPipelineLayout& GetGLPipelineLayout() const;
        const GLPipeline& GetGLPipeline() const;

        const std::vector<GLDescriptorBinding>& GetDescriptorBinding() const;
        const std::vector<VkPushConstantRange>& GetPushConstantBinding() const;

    private:
        std::unique_ptr<GLDescriptorSetLayout> glDescriptorSetLayout;
        std::vector<VkPushConstantRange> pushConstantRanges;
        std::unique_ptr<GLPipelineLayout> glPipelineLayout;
        std::unique_ptr<GLPipeline> glPipeline;
    };
}
