#include "GLBuffer.h"

#include <stdexcept>

#include "../GL.h"
#include "../Pipeline/GLCommandBuffer.h"

GLBuffer* GLBuffer::CreateTransmitter(const void* data, const size_t size)
{
    GLBuffer* glBuffer = new GLBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* bufferMemory = glBuffer->MapMemory();
    memcpy(bufferMemory, data, size);
    glBuffer->UnmapMemory();

    return glBuffer;
}
std::unique_ptr<GLBuffer> GLBuffer::CreateUniformBuffer(size_t size)
{
    return std::make_unique<GLBuffer>(
        size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | //声明缓冲区内容支持映射到主机内存
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT //声明无需主机显式调用缓存管理命令，由系统在映射前后自动同步缓冲区与主机内存，
    );
}

GLBuffer::GLBuffer(const size_t size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties)
    : size(size)
{
    //创建缓冲区
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage; //使用途径
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //队列共享模式，一般都由图形队列独占，故不需要共享
    if (vkCreateBuffer(GL::glDevice->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        throw std::runtime_error("创建缓冲区失败!");

    //获取此种缓冲区的内存需求
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(GL::glDevice->device, buffer, &memRequirements);

    //分配缓冲区内存
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = GL::glDevice->FindMemoryType(memRequirements.memoryTypeBits, properties);
    if (VkResult result = vkAllocateMemory(GL::glDevice->device, &allocInfo, nullptr, &bufferMemory); result != VK_SUCCESS)
        throw std::runtime_error("分配缓冲区内存失败!");

    //绑定内存与缓冲区
    vkBindBufferMemory(GL::glDevice->device, buffer, bufferMemory, 0);
}
GLBuffer::GLBuffer(const int size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties, const void* data)
    : GLBuffer(size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, properties)
{
    CopyFrom(data);
}
GLBuffer::~GLBuffer()
{
    vkDestroyBuffer(GL::glDevice->device, buffer, nullptr);
    vkFreeMemory(GL::glDevice->device, bufferMemory, nullptr);
}

void* GLBuffer::MapMemory() const
{
    void* result;
    vkMapMemory(GL::glDevice->device, bufferMemory, 0, size, 0, &result);
    return result;
}
void GLBuffer::UnmapMemory() const
{
    vkUnmapMemory(GL::glDevice->device, bufferMemory);
}
void GLBuffer::CopyFrom(const void* data) const
{
    std::unique_ptr<GLBuffer> transmitter(CreateTransmitter(data, size));

    GLCommandBuffer::ExecuteSingleTimeCommands([&](const GLCommandBuffer& commandBuffer)
    {
        commandBuffer.CopyBuffer(*transmitter, *this);
    });
}
