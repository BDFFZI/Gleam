#include "Heap.h"

Heap::Heap(const int elementSize, const int chunkElementCount, const int idleChunkCount)
    : elementSize(elementSize), chunkElementCount(chunkElementCount), idleChunkCount(idleChunkCount),
      elementCount(0)
{
    UpdateHeaps();
}

int Heap::GetCount() const
{
    return elementCount;
}
void Heap::AddElement(const std::function<void(std::byte* item)>& setValue)
{
    elementCount += 1;
    UpdateHeaps();

    if (setValue != nullptr)
        setValue(At(elementCount - 1));
}

void Heap::AddElements(const int count, const std::function<void(int itemIndex, std::byte* item)>& setValue)
{
    elementCount += count;
    UpdateHeaps();

    if (setValue != nullptr)
        ForeachElements(elementCount - count, count, setValue);
}
void Heap::RemoveElement(const int index)
{
    memcpy(At(index), At(elementCount - 1), elementSize);
    elementCount--;
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
    UpdateHeaps();
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
        std::byte* headAddress = heaps[heapIndex].data() + static_cast<uint32_t>(heapElementIndex * elementSize);
        int sequentialElementCount = chunkElementCount - heapElementIndex;
        int foreachCount = std::min(count, sequentialElementCount);
        //遍历元素
        for (int i = 0; i < foreachCount; i++)
        {
            iterator(foreachIndex, headAddress + static_cast<uint32_t>(i * elementSize));
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
        memcpy(destination + static_cast<uint32_t>(itemIndex * elementSize), item, elementSize);
    });
}
std::byte* Heap::At(const int index)
{
    int heapIndex;
    int heapElementIndex;
    GetHeapIndex(index, &heapIndex, &heapElementIndex);
    return heaps[heapIndex].data() + static_cast<uint32_t>(heapElementIndex * elementSize);
}

void Heap::UpdateHeaps()
{
    int occupiedChunkCount = elementCount / chunkElementCount + 1;
    int expectedChunkCount = occupiedChunkCount + idleChunkCount;

    if (static_cast<int>(heaps.size()) >= expectedChunkCount)
    {
        heaps.resize(expectedChunkCount);
    }
    else
    {
        for (int i = expectedChunkCount - static_cast<int>(heaps.size()); i > 0; i--)
            heaps.emplace_back(elementSize * chunkElementCount);
    }
}
void Heap::GetHeapIndex(const int elementIndex, int* heapIndex, int* heapElementIndex) const
{
    *heapIndex = elementIndex / chunkElementCount;
    *heapElementIndex = elementIndex % chunkElementCount;
}
