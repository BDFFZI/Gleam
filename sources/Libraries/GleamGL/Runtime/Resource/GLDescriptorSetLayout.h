#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLDescriptorBinding
{
    VkDescriptorType descriptorType;
    VkShaderStageFlags stageFlags; //该描述符在哪些着色器阶段可用

    GLDescriptorBinding(VkDescriptorType descriptorType, VkShaderStageFlags stageFlags);
};

/**
 * \brief 用于描述图形管道要绑定的描述符集的布局信息
 */
class GLDescriptorSetLayout
{
public:
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<GLDescriptorBinding> descriptorBindings;

    explicit GLDescriptorSetLayout(const std::vector<GLDescriptorBinding>& descriptorBindings, VkDescriptorSetLayoutCreateFlags createFlags = 0);
    ~GLDescriptorSetLayout();

    GLDescriptorSetLayout(const GLDescriptorSetLayout& other) = delete;
    GLDescriptorSetLayout& operator=(const GLDescriptorSetLayout& other) = delete;
    GLDescriptorSetLayout(GLDescriptorSetLayout&& other) noexcept;
    GLDescriptorSetLayout& operator=(GLDescriptorSetLayout&& other) noexcept;
};