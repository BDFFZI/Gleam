#pragma once
#include <functional>
#include <vector>

class Heap
{
public:
    Heap(int elementSize, int chunkElementCount = 64, int idleChunkCount = 1);

    int GetCount() const;

    void AddElement(const std::function<void(std::byte* item)>& setValue);
    void AddElements(int count, const std::function<void(int itemIndex, std::byte* item)>& setValue = nullptr);
    void RemoveElement(int index);
    void RemoveElements(int index, int count);
    void ForeachElements(int index, int count, const std::function<void(int itemIndex, std::byte* item)>& iterator);
    void CopyTo(std::byte* destination, int index, int count);
    std::byte* At(int index);

private:
    int elementSize;
    int chunkElementCount;
    int idleChunkCount;

    std::vector<std::vector<std::byte>> heaps;
    int elementCount;

    void UpdateHeaps();
    void GetHeapIndex(int elementIndex, int* heapIndex, int* heapElementIndex) const;
};
