#include "GLPipeline.h"

#include <functional>
#include <stdexcept>

#include "../GL.h"

VkPipeline CreatePipeline(
    const std::vector<GLShader>& glShaderLayout,
    const GLMeshLayout& glMeshLayout,
    const GLPipelineLayout& glPipelineLayout,
    const StateLayout& stateLayout,
    const std::function<void(VkGraphicsPipelineCreateInfo& pipelineCreateInfo)>& modifyPipelineCreateInfo)
{
    //着色器状态
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages(glShaderLayout.size());
    {
        for (size_t i = 0; i < glShaderLayout.size(); ++i)
        {
            VkShaderModule shaderModule;
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = glShaderLayout[i].shaderBytecode.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(glShaderLayout[i].shaderBytecode.data());
            if (vkCreateShaderModule(GL::glDevice->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
                throw std::runtime_error("创建着色器模型失败!");

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = glShaderLayout[i].shaderStage;
            vertShaderStageInfo.pName = glShaderLayout[i].shaderEntrance.c_str();
            vertShaderStageInfo.module = shaderModule;

            shaderStages[i] = vertShaderStageInfo;
        }
    }
    //顶点布局状态
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = glMeshLayout.vertexInputInfo;
    //基元装配状态
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = glMeshLayout.inputAssembly;

    //视口和裁剪阶段状态（为了支持窗口大小实时调整，已设置这些状态为动态状态，故在渲染时设置）
    VkViewport viewport{}; //视口信息（将着色器输出到图像的映射范围）
    VkRect2D scissor{}; //裁剪信息（裁剪掉图像在裁剪范围外的像素）
    VkPipelineViewportStateCreateInfo viewportState{}; //创建视口裁剪状态信息
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.pScissors = &scissor;

    //颜色混合操作状态信息？？？啥玩意
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE; //不使用按位混合功能
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &stateLayout.colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    //特殊可变状态，用户可申请部分管线状态为修改，因为有些状态确实经常更改，如果为此每次都重建管线就很不适合
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(stateLayout.dynamicStates.size());
    dynamicState.pDynamicStates = stateLayout.dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    //着色器阶段信息
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();
    //顶点输入装配信息
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    //描述符集与推送常量布局信息
    pipelineCreateInfo.layout = glPipelineLayout.pipelineLayout;
    //管线状态设置
    pipelineCreateInfo.pViewportState = &viewportState; //视口裁剪信息
    pipelineCreateInfo.pMultisampleState = &stateLayout.multisample; //多重采样信息
    pipelineCreateInfo.pRasterizationState = &stateLayout.rasterization; //光栅化信息
    pipelineCreateInfo.pDepthStencilState = &stateLayout.depthStencil; //深度模板信息
    pipelineCreateInfo.pColorBlendState = &colorBlending; //颜色混合信息
    pipelineCreateInfo.pDynamicState = &dynamicState; //需要支持动态修改的状态信息
    //不使用管线派生功能
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineCreateInfo.basePipelineIndex = -1; // Optional

    modifyPipelineCreateInfo(pipelineCreateInfo);

    VkPipeline pipeline;
    VkResult result = vkCreateGraphicsPipelines(GL::glDevice->device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);

    //回收着色器的包装器内存
    for (auto& shaderStage : shaderStages)
        vkDestroyShaderModule(GL::glDevice->device, shaderStage.module, nullptr);

    /// VK_ERROR_UNKNOWN的原因
    /// 1. 描述符集布局与着色器所使用的不一致
    if (result != VK_SUCCESS)
        throw std::runtime_error("创建图形管线失败!");

    return pipeline;
}

GLPipeline::GLPipeline(
    const GLRenderPass& glRenderPass, const int subpassIndex,
    const std::vector<GLShader>& glShaderLayout, const GLMeshLayout& glMeshLayout, const GLPipelineLayout& glPipelineLayout,
    const StateLayout& stateLayout)
{
    pipeline = CreatePipeline(
        glShaderLayout, glMeshLayout, glPipelineLayout, stateLayout,
        [&](VkGraphicsPipelineCreateInfo& pipelineCreateInfo)
        {
            pipelineCreateInfo.renderPass = glRenderPass.renderPass;
            pipelineCreateInfo.subpass = subpassIndex;
        });
}
GLPipeline::GLPipeline(
    const std::vector<VkFormat>& colorAttachments, const VkFormat depthStencilAttachment,
    const std::vector<GLShader>& glShaderLayout, const GLMeshLayout& glMeshLayout, const GLPipelineLayout& glPipelineLayout,
    const StateLayout& stateLayout)
{
    VkPipelineRenderingCreateInfo pipelineRenderingCreate = {};
    pipelineRenderingCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipelineRenderingCreate.pNext = VK_NULL_HANDLE;
    pipelineRenderingCreate.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
    pipelineRenderingCreate.pColorAttachmentFormats = colorAttachments.data();
    pipelineRenderingCreate.depthAttachmentFormat = depthStencilAttachment;
    pipelineRenderingCreate.stencilAttachmentFormat = depthStencilAttachment;

    pipeline = CreatePipeline(
        glShaderLayout, glMeshLayout, glPipelineLayout, stateLayout,
        [&](VkGraphicsPipelineCreateInfo& pipelineCreateInfo)
        {
            pipelineCreateInfo.renderPass = VK_NULL_HANDLE;
            pipelineCreateInfo.pNext = &pipelineRenderingCreate;
        });
}
GLPipeline::~GLPipeline()
{
    vkDestroyPipeline(GL::glDevice->device, pipeline, nullptr);
}
