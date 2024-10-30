#pragma once
#include <functional>
#include <vector>
#include <memory>

class Heap
{
public:
    Heap() = default;
    Heap(size_t elementSize, int chunkElementCount = 64, int idleChunkCount = 1);

    int GetCount() const;

    void AddElement(const std::function<void(std::byte* item)>& setValue);
    void AddElements(int count, const std::function<void(int itemIndex, std::byte* item)>& setValue = nullptr);
    void RemoveElement(int index);
    void RemoveElements(int index, int count);
    void ForeachElements(int index, int count, const std::function<void(int itemIndex, std::byte* item)>& iterator);
    void ForeachElements(const std::function<void(std::byte* item)>& iterator);
    void CopyTo(std::byte* destination, int index, int count);
    std::byte* At(int index);

private:
    size_t elementSize;
    int chunkElementCount;
    int idleChunkCount;

    std::vector<std::unique_ptr<std::byte[]>> heaps;
    int elementCount;

    void UpdateHeaps();
    void GetHeapIndex(int elementIndex, int* heapIndex, int* heapElementIndex) const;
};
