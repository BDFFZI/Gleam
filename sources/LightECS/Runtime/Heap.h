#pragma once
#include <functional>
#include <vector>
#include <memory>

/**
 * @brief 一种优化的内存分配容器
 */
class Heap
{
public:
    Heap() = default;
    Heap(size_t elementSize, int chunkElementCount = 64, int spareChunkCount = 1);

    int GetCount() const;

    void AddElement(const std::function<void(std::byte* item)>& setValue);
    void AddElements(int count, const std::function<void(int itemIndex, std::byte* item)>& setValue = nullptr);
    void RemoveElement(int index);
    void RemoveElements(int index, int count);
    void ForeachElements(int index, int count, const std::function<void(int itemIndex, std::byte* item)>& iterator);
    template <typename TIterator> requires requires(TIterator iterator, std::byte* ptr) { iterator(ptr); }
    void ForeachElements(TIterator iterator)
    {
        int count = elementCount;
        int heapIndex = 0;
        while (count > 0)
        {
            //获取遍历地址和最大遍历次数
            std::byte* begin = heaps[heapIndex].get();
            std::byte* end = begin + std::min(count, chunkElementCount) * elementSize;
            //遍历元素
            for (std::byte* item = begin; item != end; item += elementSize)
                iterator(item);
            //获取下次遍历的信息
            heapIndex++;
            count -= chunkElementCount;
        }
    }
    void CopyTo(std::byte* destination, int index, int count);
    std::byte* At(int index);

private:
    size_t elementSize;
    int chunkElementCount;
    int spareChunkCount;

    std::vector<std::unique_ptr<std::byte[]>> heaps;
    int elementCount;

    void ResizeHeaps();
    void GetHeapIndex(int elementIndex, int* heapIndex, int* heapElementIndex) const;
};
