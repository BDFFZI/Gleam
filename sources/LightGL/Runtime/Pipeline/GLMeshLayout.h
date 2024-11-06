#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLVertexAttribute
{
    uint32_t location;
    uint32_t offset;
    VkFormat format;

    GLVertexAttribute(uint32_t location, uint32_t offset, VkFormat format);
};

struct GLMeshLayout
{
    VkVertexInputBindingDescription bindingDescription = {};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {};
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};

    GLMeshLayout(uint32_t vertexSize, const std::vector<GLVertexAttribute>& vertexAttributes, VkPrimitiveTopology primitiveTopology);
};
