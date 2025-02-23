#pragma once
#include "../Resource/GLDescriptorSetLayout.h"

struct GLPipelineLayoutData
{
    ~GLPipelineLayoutData();
};

/**
 * 描述符集和推送常量布局状态（常量缓冲区，纹理等资源在着色器中的布局）
 */
class GLPipelineLayout
{
public:
    VkPipelineLayout pipelineLayout;

    explicit GLPipelineLayout(const GLDescriptorSetLayout& glDescriptorSetLayout, const std::vector<VkPushConstantRange>& pushConstantRanges = {});
    ~GLPipelineLayout();

    GLPipelineLayout(const GLPipelineLayout& other) = delete;
    GLPipelineLayout& operator=(const GLPipelineLayout& other) = delete;
    GLPipelineLayout(GLPipelineLayout&& other) noexcept;
    GLPipelineLayout& operator=(GLPipelineLayout&& other) noexcept;
};
