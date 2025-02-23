#include "GLPipeline.h"

#include <functional>
#include <stdexcept>

#include "../GL.h"

VkPipeline CreatePipeline(
    const GLShaderLayout& glShaderLayout,
    const GLMeshLayout& glMeshLayout,
    const GLPipelineLayout& glPipelineLayout,
    const GLStateLayout& glStateLayout,
    const std::function<void(VkGraphicsPipelineCreateInfo& pipelineCreateInfo)>& modifyPipelineCreateInfo)
{
    //颜色混合操作状态信息？？？啥玩意
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE; //不使用按位混合功能
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &glStateLayout.colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    //特殊可变状态，用户可申请部分管线状态为修改，因为有些状态确实经常更改，如果为此每次都重建管线就很不适合
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(glStateLayout.dynamicStates.size());
    dynamicState.pDynamicStates = glStateLayout.dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    //着色器阶段信息
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(glShaderLayout.shaderStages.size());
    pipelineCreateInfo.pStages = glShaderLayout.shaderStages.data();
    //顶点输入装配信息
    pipelineCreateInfo.pVertexInputState = &glMeshLayout.vertexInput.vertexInputState; //顶点布局状态
    pipelineCreateInfo.pInputAssemblyState = &glMeshLayout.inputAssembly.inputAssemblyState;//基元装配状态
    //描述符集与推送常量布局信息
    pipelineCreateInfo.layout = glPipelineLayout.pipelineLayout;
    //管线状态设置
    pipelineCreateInfo.pViewportState = &glStateLayout.viewport; //视口裁剪信息
    pipelineCreateInfo.pMultisampleState = &glStateLayout.multisample; //多重采样信息
    pipelineCreateInfo.pRasterizationState = &glStateLayout.rasterization; //光栅化信息
    pipelineCreateInfo.pDepthStencilState = &glStateLayout.depthStencil; //深度模板信息
    pipelineCreateInfo.pColorBlendState = &colorBlending; //颜色混合信息
    pipelineCreateInfo.pDynamicState = &dynamicState; //需要支持动态修改的状态信息
    //不使用管线派生功能
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineCreateInfo.basePipelineIndex = -1; // Optional

    modifyPipelineCreateInfo(pipelineCreateInfo);

    VkPipeline pipeline;
    VkResult result = vkCreateGraphicsPipelines(GL::glDevice->device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);

    /// VK_ERROR_UNKNOWN的原因
    /// 1. 描述符集布局与着色器所使用的不一致
    if (result != VK_SUCCESS)
        throw std::runtime_error("创建图形管线失败!");

    return pipeline;
}

GLPipeline::GLPipeline(
    const GLRenderPass& glRenderPass, const int subpassIndex,
    const GLShaderLayout& glShaderLayout, const GLMeshLayout& glMeshLayout, const GLPipelineLayout& glPipelineLayout,
    const GLStateLayout& glStateLayout)
{
    pipeline = CreatePipeline(
        glShaderLayout, glMeshLayout, glPipelineLayout, glStateLayout,
        [&](VkGraphicsPipelineCreateInfo& pipelineCreateInfo)
        {
            pipelineCreateInfo.renderPass = glRenderPass.renderPass;
            pipelineCreateInfo.subpass = subpassIndex;
        });
}
GLPipeline::GLPipeline(
    const std::vector<VkFormat>& colorAttachments, const VkFormat depthStencilAttachment,
    const GLShaderLayout& glShaderLayout, const GLMeshLayout& glMeshLayout, const GLPipelineLayout& glPipelineLayout,
    const GLStateLayout& glStateLayout)
{
    VkPipelineRenderingCreateInfo pipelineRenderingCreate = {};
    pipelineRenderingCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipelineRenderingCreate.pNext = VK_NULL_HANDLE;
    pipelineRenderingCreate.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
    pipelineRenderingCreate.pColorAttachmentFormats = colorAttachments.data();
    pipelineRenderingCreate.depthAttachmentFormat = depthStencilAttachment;
    pipelineRenderingCreate.stencilAttachmentFormat = depthStencilAttachment;

    pipeline = CreatePipeline(
        glShaderLayout, glMeshLayout, glPipelineLayout, glStateLayout,
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
