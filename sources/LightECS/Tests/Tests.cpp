#include <array>
#include <iostream>
#include <ostream>
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include "LightECS/Runtime/Heap.h"
#include "LightECS/Runtime/World.h"
#include <source_location>
#include <unordered_set>

#ifdef BENCHMARK_Heap
struct Data
{
    float data[32];
};
void VectorTest(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<Data> container;
        container.resize(30);
        container.resize(60);
        int size = container.size();
        for (int i = 0; i < size; i++)
            container[i].data[0] = i;
        container.erase(container.begin(), container.begin() + 30);

        container.resize(90);
        container.resize(120);
        size = container.size();
        for (int i = 0; i < size; i++)
            container[i].data[1] = i;
        container.erase(container.begin() + 30, container.begin() + 60);

        for (int i = 0; i < container.size(); i++)
        {
            if (i % 2 == 0)
                container.erase(container.begin() + i);
        }
    }
}
void HeapTest(benchmark::State& state)
{
    for (auto _ : state)
    {
        Heap container(sizeof(Data));
        container.AddElements(30);
        container.AddElements(30);
        container.ForeachElements(0, container.GetCount(), [](const int index, std::byte* ptr)
        {
            Data* data = reinterpret_cast<Data*>(ptr);
            data->data[0] = index;
        });
        container.RemoveElements(0, 30);

        container.AddElements(30);
        container.AddElements(30);
        container.ForeachElements(0, container.GetCount(), [](const int index, std::byte* ptr)
        {
            Data* data = reinterpret_cast<Data*>(ptr);
            data->data[1] = index;
        });
        container.RemoveElements(30, 30);

        for (int i = 0; i < container.GetCount(); i++)
        {
            if (i % 2 == 0)
                container.RemoveElement(i);
        }
    }
}
BENCHMARK(VectorTest);
BENCHMARK(HeapTest);
BENCHMARK_MAIN();
#endif

TEST(ECS, Heap)
{
    Heap heap(sizeof(int));

    heap.AddElements(5, [](const int itemIndex, std::byte* item)
    {
        int* element = reinterpret_cast<int*>(item);
        *element = itemIndex;
    });

    heap.RemoveElements(2, 2);
    heap.RemoveElement(1);
    heap.AddElement([](std::byte* item)
    {
        *reinterpret_cast<int*>(item) = 5;
    });
    *reinterpret_cast<int*>(heap.At(0)) = 3;

    std::vector<int> vector(heap.GetCount());
    heap.CopyTo(reinterpret_cast<std::byte*>(vector.data()), 0, heap.GetCount());
    ASSERT_EQ(vector.size(), 3);
    ASSERT_EQ(vector[0], 3);
    ASSERT_EQ(vector[1], 4);
    ASSERT_EQ(vector[2], 5);
}

struct Position
{
    float x;
    float y;
    float z;
};
struct Velocity
{
    float x;
    float y;
};


constinit Archetype physicalArchetype = RegisterArchetype<Position, Velocity>();
constinit Archetype physicalArchetypeWithDrug = RegisterArchetype<int>(physicalArchetype);

void main()
{
    std::cout << to_string(physicalArchetypeWithDrug) << "\n";
    std::cout << physicalArchetypeWithDrug.Contains<Position>() << "\n";
    std::cout << physicalArchetypeWithDrug.Contains<Position, int>() << "\n";
    std::cout << physicalArchetypeWithDrug.Contains<bool>() << "\n";
    std::cout << physicalArchetypeWithDrug.Contains<bool, int>() << "\n";
}
