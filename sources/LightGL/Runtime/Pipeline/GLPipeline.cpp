#include "GLPipeline.h"

#include <stdexcept>

#include "../Foundation/GLFoundation.h"

GLPipeline::GLPipeline(const GLRenderPass& glRenderPass, int subpassIndex,
                       const std::vector<GLShader>& glShaderLayout, const GLMeshLayout& glMeshLayout, const GLPipelineLayout& glPipelineLayout)
{
    //着色器状态
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages(glShaderLayout.size());
    {
        for (size_t i = 0; i < glShaderLayout.size(); ++i)
        {
            VkShaderModule shaderModule;
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = glShaderLayout[i].shaderCode.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(glShaderLayout[i].shaderCode.data());
            if (vkCreateShaderModule(GLFoundation::glDevice->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
                throw std::runtime_error("创建着色器模型失败!");

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = glShaderLayout[i].shaderStage;
            vertShaderStageInfo.pName = glShaderLayout[i].shaderName.c_str();
            vertShaderStageInfo.module = shaderModule;

            shaderStages[i] = vertShaderStageInfo;
        }
    }
    //顶点布局状态
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = glMeshLayout.vertexInputInfo;
    //基元装配状态
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = glMeshLayout.inputAssembly;


    //视口和裁剪阶段状态
    VkViewport viewport{}; //视口信息（将着色器输出到图像的映射范围）
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = 0;
    viewport.height = 0;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{}; //裁剪信息（裁剪掉图像在裁剪范围外的像素）
    scissor.offset = {0, 0};
    scissor.extent = {0, 0};
    VkPipelineViewportStateCreateInfo viewportState{}; //创建视口裁剪状态信息
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.pScissors = &scissor;

    //光栅化阶段状态
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE; //是否将深度钳制在0-1而不是剔除超出部分。
    rasterizer.rasterizerDiscardEnable = VK_FALSE; //是否在栅格化阶段之前丢弃基元
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL; //多边形需要填充
    rasterizer.cullMode = VK_CULL_MODE_NONE; //需要背面剔除
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; //多边形正反面判断按顺时针顶点方向
    rasterizer.depthBiasEnable = VK_FALSE; //关闭深度偏移
    rasterizer.depthBiasConstantFactor = 0.0f; //以常值进行深度偏移
    rasterizer.depthBiasSlopeFactor = 0.0f; //以多边形相对相机z轴斜率进行深度偏移
    rasterizer.depthBiasClamp = 0.0f; //深度偏移的最大范围
    rasterizer.lineWidth = 1.0f; //基元为线时的宽度

    //多重采样阶段状态（MSAA，常用于实现抗锯齿）
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //不使用多重采样，故仅采样一次
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //深度模板测试阶段状态
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE; //启用深度测试
    depthStencil.depthWriteEnable = VK_TRUE; //启用深度写入
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; //深度测试通过条件为小于
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // 最小深度
    depthStencil.maxDepthBounds = 1.0f; // 最大深度
    depthStencil.stencilTestEnable = VK_FALSE; //不使用模板测试
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    //缓冲区混合阶段状态
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    //所有颜色通道都可写入
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    //不使用混合功能
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; //输出颜色系数
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; //缓冲区颜色系数
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; //颜色混合方式，一般都为相加
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //输出透明度系数
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //缓冲区透明度系数
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; //透明度混合方式

    //颜色混合操作状态信息？？？啥玩意
    VkPipelineColorBlendStateCreateInfo colorBlending{};

    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE; //不使用按位混合功能
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    //特殊可变状态，用户可申请部分管线状态为修改，因为有些状态确实经常更改，如果为此每次都重建管线就很不适合
    const std::vector dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    }; //视口和裁剪参数的调整是非常普遍的，因为窗口大小可以调整，故设为动态状态
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

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
    pipelineCreateInfo.pRasterizationState = &rasterizer; //光栅化信息
    pipelineCreateInfo.pMultisampleState = &multisampling; //多重采样信息
    pipelineCreateInfo.pDepthStencilState = &depthStencil; //深度模板信息
    pipelineCreateInfo.pColorBlendState = &colorBlending; //颜色混合信息
    pipelineCreateInfo.pDynamicState = &dynamicState; //需要支持动态修改的状态信息
    //是哪个subpass对应的管道
    pipelineCreateInfo.renderPass = glRenderPass.renderPass;
    pipelineCreateInfo.subpass = subpassIndex;
    //不使用管线派生功能
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineCreateInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(GLFoundation::glDevice->device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline) != VK_SUCCESS)
        throw std::runtime_error("创建图形管线失败!");

    //回收着色器的包装器内存
    for (auto& shaderStage : shaderStages)
        vkDestroyShaderModule(GLFoundation::glDevice->device, shaderStage.module, nullptr);
}
GLPipeline::~GLPipeline()
{
    vkDestroyPipeline(GLFoundation::glDevice->device, pipeline, nullptr);
}
