#include "GLDescriptorPool.h"

#include <stdexcept>
#include <unordered_map>

#include "../GL.h"


GLDescriptorPool::GLDescriptorPool(const GLDescriptorSetLayout& descriptorSetLayout, const int descriptorSetCount, VkDescriptorPoolCreateFlags createFlags)
{
    //计算可分配的描述符类型及其最大数量
    std::vector<VkDescriptorPoolSize> poolSizes = {};
    {
        std::unordered_map<VkDescriptorType, int> descriptorPoolSizes = {};
        for (const auto& descriptorBinding : descriptorSetLayout.descriptorBindings)
            descriptorPoolSizes[descriptorBinding.descriptorType] += descriptorSetCount;

        for (const auto& [descriptorType,count] : descriptorPoolSizes)
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = descriptorType;
            poolSize.descriptorCount = count;
            poolSizes.push_back(poolSize);
        }
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    //描述符数量
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    //描述符集数量
    poolInfo.maxSets = static_cast<uint32_t>(descriptorSetCount);
    poolInfo.flags = createFlags;

    if (vkCreateDescriptorPool(GL::glDevice->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("创建描述符集池失败!");
}

GLDescriptorPool::~GLDescriptorPool()
{
    vkDestroyDescriptorPool(GL::glDevice->device, descriptorPool, nullptr);
}
