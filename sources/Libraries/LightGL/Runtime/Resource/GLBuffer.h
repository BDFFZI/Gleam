#pragma once

#include "../Foundation/GLDevice.h"
#include <memory>

class GLBuffer
{
public:
    /**
     * 具有与CPU内存同步能力的缓冲区无法发挥最佳性能，故需要创建仅GPU可用的缓冲区。
     * 而为了向此类缓冲区提供数据，就需要一个支持CPU写入的临时缓冲区做为中间人。
     * 然后再通过GPU命令，在GPU中将其内容传输到仅GPU可用的缓冲区中。
     * @param data 
     * @param size 
     * @return 支持CPU内存写入且可做GPU传输源的临时缓冲区
     */
    static GLBuffer* CreateTransmitter(const void* data, size_t size);
    static std::unique_ptr<GLBuffer> CreateUniformBuffer(size_t size);

    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    size_t size;

    GLBuffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    /**
    * 自动添加VK_BUFFER_USAGE_TRANSFER_DST_BIT，并传入初始值
    * @param size 
    * @param usage 
    * @param properties 
    * @param data 
    * @return 
    */
    GLBuffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const void* data);
    GLBuffer(const GLBuffer&) = delete;
    ~GLBuffer();

    /**
     * 需确保缓冲区支持VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
     * @return 将GPU内存映射到CPU后的内存地址
     */
    void* MapMemory() const;
    /**
    * 配合关闭MapMemory()功能
    */
    void UnmapMemory() const;
    /**
     * 需保证缓冲区支持VK_BUFFER_USAGE_TRANSFER_DST_BIT
     * @param data 
     */
    void CopyFrom(const void* data) const;
};
