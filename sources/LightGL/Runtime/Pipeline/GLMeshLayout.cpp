#include "GLMeshLayout.h"

GLVertexAttribute::GLVertexAttribute(const uint32_t location, const uint32_t offset, const VkFormat format)
    : location(location), offset(offset), format(format)
{
}

GLMeshLayout::GLMeshLayout(const uint32_t vertexSize, const std::vector<GLVertexAttribute>& vertexAttributes, const VkPrimitiveTopology primitiveTopology)
{
    bindingDescription.binding = 0; //管道的顶点缓冲区允许绑定多个，我们只绑定一个，对应索引为0
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //输入的是顶点而不是实例数据
    bindingDescription.stride = vertexSize; //每段数据的大小

    attributeDescriptions.resize(vertexAttributes.size());
    for (uint32_t i = 0; i < attributeDescriptions.size(); i++)
    {
        VkVertexInputAttributeDescription attributeDescription;
        attributeDescription.binding = 0; //管道的顶点缓冲区允许绑定多个，我们只绑定一个，对应索引为0
        attributeDescription.location = vertexAttributes[i].location; //对应在shader中的顶点属性槽位
        attributeDescription.format = vertexAttributes[i].format;
        attributeDescription.offset = vertexAttributes[i].offset;
        attributeDescriptions[i] = attributeDescription;
    }

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    //输入装配信息（如何装配成基元，如三角面）
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = primitiveTopology;
    inputAssembly.primitiveRestartEnable = VK_FALSE; //如果是条带模式，则是否允许断开
}
