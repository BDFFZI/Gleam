#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLVertexAttribute
{
    uint32_t location;
    uint32_t offset;
    VkFormat format;
};

struct GLMeshVertexLayout
{
    std::vector<VkVertexInputBindingDescription2EXT> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription2EXT> vertexInputAttributes;

    GLMeshVertexLayout(uint32_t vertexSize, std::vector<GLVertexAttribute> vertexAttributes);
};

struct GLMeshLayout
{
    VkVertexInputBindingDescription bindingDescription = {};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {};
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};


    GLMeshLayout(const GLMeshVertexLayout& vertexLayout, VkPrimitiveTopology indexLayout);
};
