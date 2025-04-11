#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include "GleamUtility/Runtime/Container/Heap.h"

using namespace Gleam;

TEST(Heap, Edit)
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

TEST(Heap, HeapBenchmark)
{
    struct Data
    {
        size_t data[32];
    };

    benchmark::RegisterBenchmark("Vector", [](benchmark::State& state)
    {
        for (auto _ : state)
        {
            std::vector<Data> container;
            container.resize(30);
            container.resize(60);
            size_t size = container.size();
            for (size_t i = 0; i < size; i++)
                container[i].data[0] = i;
            container.erase(container.begin(), container.begin() + 30);

            container.resize(90);
            container.resize(120);
            size = container.size();
            for (size_t i = 0; i < size; i++)
                container[i].data[1] = i;
            container.erase(container.begin() + 30, container.begin() + 60);

            for (size_t i = 0; i < container.size(); i++)
            {
                if (i % 2 == 0)
                    container.erase(container.begin() + static_cast<int64_t>(i));
            }
        }
    });
    benchmark::RegisterBenchmark("Heap", [](benchmark::State& state)
    {
        for (auto _ : state)
        {
            Heap container(sizeof(Data));
            container.AddElements(30);
            container.AddElements(30);
            int index = 0;
            container.ForeachElements([&index](std::byte* ptr)
            {
                Data* data = reinterpret_cast<Data*>(ptr);
                data->data[0] = index;
            });
            container.RemoveElements(0, 30);

            container.AddElements(30);
            container.AddElements(30);
            index = 0;
            container.ForeachElements([&index](std::byte* ptr)
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
    });
    benchmark::Initialize(nullptr, nullptr);
    benchmark::RunSpecifiedBenchmarks();
}

TEST(Heap, Move)
{
    Heap a = {};
    Heap b = {};
    a = std::move(b);
    // b = a;
}