#include "GLCommandPool.h"
#include <stdexcept>

GLCommandPool::GLCommandPool(const GLDevice& glDevice)
    : device(glDevice.device)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; //分配出的命令缓冲区允许重置复用
    poolInfo.queueFamilyIndex = glDevice.graphicQueueFamily; //针对图形队列族的命令池

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        throw std::runtime_error("命令池创建失败!");
}
GLCommandPool::~GLCommandPool()
{
    vkDestroyCommandPool(device, commandPool, nullptr);
}
