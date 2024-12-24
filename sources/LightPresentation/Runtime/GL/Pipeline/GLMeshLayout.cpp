#include "GLMeshLayout.h"

GLVertexInput::GLVertexInput(const uint32_t vertexSize, const std::vector<GLVertexAttribute>& vertexAttributes)
{
    VkVertexInputBindingDescription2EXT bindingDescription2EXT{};
    bindingDescription2EXT.binding = 0; //管道的顶点缓冲区允许绑定多个，我们只绑定一个，对应索引为0
    bindingDescription2EXT.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //输入的是顶点而不是实例数据
    bindingDescription2EXT.stride = vertexSize; //每段数据的大小
    bindingDescription2EXTs.push_back(bindingDescription2EXT);

    attributeDescription2EXTs.resize(vertexAttributes.size());
    for (uint32_t i = 0; i < attributeDescription2EXTs.size(); i++)
    {
        VkVertexInputAttributeDescription2EXT vertexInputAttribute = {};
        vertexInputAttribute.binding = 0; //管道的顶点缓冲区允许绑定多个，我们只绑定一个，对应索引为0
        vertexInputAttribute.location = vertexAttributes[i].location; //对应在shader中的顶点属性槽位
        vertexInputAttribute.format = vertexAttributes[i].format;
        vertexInputAttribute.offset = vertexAttributes[i].offset;
        attributeDescription2EXTs[i] = vertexInputAttribute;
    }

    bindingDescriptions.reserve(bindingDescription2EXTs.size());
    for (auto& vertexInputBinding : bindingDescription2EXTs)
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = vertexInputBinding.binding;
        bindingDescription.inputRate = vertexInputBinding.inputRate;
        bindingDescription.stride = vertexInputBinding.stride;
        bindingDescriptions.push_back(bindingDescription);
    }

    attributeDescriptions.reserve(attributeDescription2EXTs.size());
    for (auto& vertexInputAttribute : attributeDescription2EXTs)
    {
        VkVertexInputAttributeDescription attributeDescription = {};
        attributeDescription.binding = vertexInputAttribute.binding;
        attributeDescription.location = vertexInputAttribute.location;
        attributeDescription.format = vertexInputAttribute.format;
        attributeDescription.offset = vertexInputAttribute.offset;
        attributeDescriptions.push_back(attributeDescription);
    }

    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputState.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.data();
}
GLInputAssembly::GLInputAssembly(const VkPrimitiveTopology primitiveTopology, const bool primitiveRestartEnable)
{
    //输入装配信息（如何装配成基元，如三角面）
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = primitiveTopology;
    inputAssemblyState.primitiveRestartEnable = primitiveRestartEnable; //如果是条带模式，则是否允许断开
}
