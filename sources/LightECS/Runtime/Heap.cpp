#include "Heap.h"

Heap::Heap(const size_t elementSize, const int chunkElementCount, const int spareChunkCount)
    : elementSize(elementSize), chunkElementCount(chunkElementCount), spareChunkCount(spareChunkCount),
      elementCount(0)
{
}

int Heap::GetCount() const
{
    return elementCount;
}
void Heap::AddElement(const std::function<void(std::byte* item)>& setValue)
{
    elementCount += 1;
    ResizeHeaps();

    if (setValue != nullptr)
        setValue(At(elementCount - 1));
}
void Heap::AddElements(const int count, const std::function<void(int itemIndex, std::byte* item)>& setValue)
{
    elementCount += count;
    ResizeHeaps();

    if (setValue != nullptr)
        ForeachElements(elementCount - count, count, setValue);
}
std::byte* Heap::RemoveElement(const int index)
{
    std::byte* item = At(index);
    memcpy(item, At(elementCount - 1), elementSize);
    elementCount--;
    return item;
}
void Heap::RemoveElements(const int index, const int count)
{
    int surplusCount = elementCount - index - count;
    int foreachCount = std::min(surplusCount, elementCount);

    ForeachElements(index, foreachCount, [this,surplusIndex = index + count](const int itemIndex, std::byte* item)
    {
        std::byte* source = At(surplusIndex + itemIndex);
        memcpy(item, source, elementSize);
    });

    elementCount -= count;
    ResizeHeaps();
}
void Heap::ForeachElements(const int index, int count, const std::function<void(int itemIndex, std::byte* item)>& iterator)
{
    int heapIndex;
    int heapElementIndex;
    GetHeapIndex(index, &heapIndex, &heapElementIndex);

    int foreachIndex = 0;
    while (true)
    {
        //获取遍历地址和最大遍历次数
        std::byte* headAddress = heaps[heapIndex].get() + heapElementIndex * elementSize;
        int sequentialElementCount = chunkElementCount - heapElementIndex;
        int foreachCount = std::min(count, sequentialElementCount);
        //遍历元素
        for (int i = 0; i < foreachCount; i++)
        {
            iterator(foreachIndex, headAddress + i * elementSize);
            foreachIndex++;
        }
        //完成一次遍历
        count -= sequentialElementCount;
        if (count <= 0)
            break;
        //获取下次遍历的信息
        GetHeapIndex(index + sequentialElementCount, &heapIndex, &heapElementIndex);
    }
}
void Heap::CopyTo(std::byte* destination, const int index, const int count)
{
    ForeachElements(index, count, [this,destination](const int itemIndex, std::byte* item)
    {
        memcpy(destination + itemIndex * elementSize, item, elementSize);
    });
}
std::byte* Heap::At(const int index)
{
    int heapIndex;
    int heapElementIndex;
    GetHeapIndex(index, &heapIndex, &heapElementIndex);
    return heaps[heapIndex].get() + heapElementIndex * elementSize;
}

void Heap::ResizeHeaps()
{
    size_t occupiedChunkCount = elementCount / chunkElementCount + 1; //必须块数
    size_t expectedChunkCount = occupiedChunkCount + spareChunkCount; //最佳块数=必须块数+备用块数

    if (heaps.size() > expectedChunkCount) //容量比最佳块数要多，触发回收
    {
        heaps.resize(expectedChunkCount); //回收至期望块数
    }
    else if (heaps.size() < occupiedChunkCount) //容量不足必须块数，触发扩容
    {
        //扩容块数量到最佳块数
        for (size_t i = expectedChunkCount - heaps.size(); i > 0; i--)
            heaps.emplace_back(new std::byte[elementSize * chunkElementCount]);
    }
}
void Heap::GetHeapIndex(const int elementIndex, int* heapIndex, int* heapElementIndex) const
{
    *heapIndex = elementIndex / chunkElementCount;
    *heapElementIndex = elementIndex % chunkElementCount;
}
