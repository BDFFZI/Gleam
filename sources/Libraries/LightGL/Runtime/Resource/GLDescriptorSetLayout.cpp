#include "GLDescriptorSetLayout.h"

#include <stdexcept>

#include "../GL.h"

GLDescriptorBinding::GLDescriptorBinding(const VkDescriptorType descriptorType, const VkShaderStageFlags stageFlags)
    : descriptorType(descriptorType),
      stageFlags(stageFlags)
{
}

GLDescriptorSetLayout::GLDescriptorSetLayout(const std::vector<GLDescriptorBinding>& descriptorBindings, const VkDescriptorSetLayoutCreateFlags createFlags)
    : descriptorBindings(descriptorBindings)
{
    //槽位绑定信息
    std::vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings(descriptorBindings.size());
    for (size_t i = 0; i < descriptorBindings.size(); i++)
    {
        VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding = {};
        vkDescriptorSetLayoutBinding.binding = static_cast<uint32_t>(i);
        vkDescriptorSetLayoutBinding.descriptorType = descriptorBindings[i].descriptorType;
        vkDescriptorSetLayoutBinding.stageFlags = descriptorBindings[i].stageFlags;
        vkDescriptorSetLayoutBinding.descriptorCount = 1; //插槽支持数组功能，故可绑定多个描述符，暂且不使用
        vkDescriptorSetLayoutBindings[i] = vkDescriptorSetLayoutBinding;
    }

    //创建图形管道的描述符布局
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(vkDescriptorSetLayoutBindings.size());
    layoutInfo.pBindings = vkDescriptorSetLayoutBindings.data();
    layoutInfo.flags = createFlags;

    if (vkCreateDescriptorSetLayout(GL::glDevice->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error("创建描述符集布局失败!");
}
GLDescriptorSetLayout::~GLDescriptorSetLayout()
{
    if (descriptorSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(GL::glDevice->device, descriptorSetLayout, nullptr);
}

GLDescriptorSetLayout::GLDescriptorSetLayout(GLDescriptorSetLayout&& other) noexcept
    : descriptorSetLayout(other.descriptorSetLayout),
      descriptorBindings(std::move(other.descriptorBindings))
{
    other.descriptorSetLayout = VK_NULL_HANDLE;
}
GLDescriptorSetLayout& GLDescriptorSetLayout::operator=(GLDescriptorSetLayout&& other) noexcept
{
    if (this == &other)
        return *this;
    descriptorSetLayout = other.descriptorSetLayout;
    descriptorBindings = std::move(other.descriptorBindings);
        
    other.descriptorSetLayout = VK_NULL_HANDLE;
    return *this;
}