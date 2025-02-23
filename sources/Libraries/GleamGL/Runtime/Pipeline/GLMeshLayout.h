#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLVertexAttribute
{
    uint32_t location;
    uint32_t offset;
    VkFormat format;
};

struct GLVertexInput
{
    std::vector<VkVertexInputBindingDescription2EXT> bindingDescription2EXTs;
    std::vector<VkVertexInputAttributeDescription2EXT> attributeDescription2EXTs;
    //创建GLPipeline所需的顶点布局状态信息
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    VkPipelineVertexInputStateCreateInfo vertexInputState = {};

    GLVertexInput(uint32_t vertexSize, const std::vector<GLVertexAttribute>& vertexAttributes);
};

struct GLInputAssembly
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};

    GLInputAssembly(VkPrimitiveTopology primitiveTopology, bool primitiveRestartEnable);
};

struct GLMeshLayout
{
    GLVertexInput vertexInput;
    GLInputAssembly inputAssembly;
};