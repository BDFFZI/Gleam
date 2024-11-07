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
    std::vector<VkVertexInputBindingDescription2EXT> vertexInputBindings2EXT;
    std::vector<VkVertexInputAttributeDescription2EXT> vertexInputAttributes2EXT;
    std::vector<VkVertexInputBindingDescription> vertexInputBindings = {};
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {};
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};

    GLMeshVertexLayout(uint32_t vertexSize, const std::vector<GLVertexAttribute>& vertexAttributes);
};

struct GLMeshIndexLayout
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};

    GLMeshIndexLayout(VkPrimitiveTopology primitiveTopology, bool primitiveRestartEnable);
};

struct GLMeshLayout
{
    GLMeshVertexLayout vertexLayout;
    GLMeshIndexLayout indexLayout;


    GLMeshLayout(const GLMeshVertexLayout& vertexLayout, VkPrimitiveTopology indexLayout);
};
