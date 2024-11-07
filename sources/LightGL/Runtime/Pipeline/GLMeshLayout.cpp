#include "GLMeshLayout.h"

GLMeshVertexLayout::GLMeshVertexLayout(const uint32_t vertexSize, const std::vector<GLVertexAttribute>& vertexAttributes)
{
    VkVertexInputBindingDescription2EXT bindingDescription{};
    bindingDescription.binding = 0; //管道的顶点缓冲区允许绑定多个，我们只绑定一个，对应索引为0
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //输入的是顶点而不是实例数据
    bindingDescription.stride = vertexSize; //每段数据的大小
    vertexInputBindings2EXT.push_back(bindingDescription);

    vertexInputAttributes2EXT.resize(vertexAttributes.size());
    for (uint32_t i = 0; i < vertexInputAttributes2EXT.size(); i++)
    {
        VkVertexInputAttributeDescription2EXT vertexInputAttribute = {};
        vertexInputAttribute.binding = 0; //管道的顶点缓冲区允许绑定多个，我们只绑定一个，对应索引为0
        vertexInputAttribute.location = vertexAttributes[i].location; //对应在shader中的顶点属性槽位
        vertexInputAttribute.format = vertexAttributes[i].format;
        vertexInputAttribute.offset = vertexAttributes[i].offset;
        vertexInputAttributes2EXT[i] = vertexInputAttribute;
    }
}
GLMeshLayout::GLMeshLayout(const GLMeshVertexLayout& vertexLayout, const VkPrimitiveTopology indexLayout)
{
    vertexInputBindings.reserve(vertexLayout.vertexInputBindings2EXT.size());
    for (auto& vertexInputBinding : vertexLayout.vertexInputBindings2EXT)
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = vertexInputBinding.binding;
        bindingDescription.inputRate = vertexInputBinding.inputRate;
        bindingDescription.stride = vertexInputBinding.stride;
        vertexInputBindings.push_back(bindingDescription);
    }

    vertexInputAttributes.resize(vertexLayout.vertexInputAttributes2EXT.size());
    for (auto& vertexInputAttribute : vertexLayout.vertexInputAttributes2EXT)
    {
        VkVertexInputAttributeDescription attributeDescription = {};
        attributeDescription.binding = vertexInputAttribute.binding;
        attributeDescription.location = vertexInputAttribute.location;
        attributeDescription.format = vertexInputAttribute.format;
        attributeDescription.offset = vertexInputAttribute.offset;
        vertexInputAttributes.push_back(attributeDescription);
    }

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
    vertexInputInfo.pVertexBindingDescriptions = vertexInputBindings.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
    vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributes.data();

    //输入装配信息（如何装配成基元，如三角面）
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = indexLayout;
    inputAssembly.primitiveRestartEnable = VK_FALSE; //如果是条带模式，则是否允许断开
}
