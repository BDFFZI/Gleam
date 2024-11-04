#pragma once
#include <cstddef>

template <typename TComponent>
class ComponentMeta
{
public:
    static void Constructor(std::byte* ptr)
    {
        new(ptr) TComponent();
    }
    static void Destructor(std::byte* ptr)
    {
        reinterpret_cast<TComponent*>(ptr)->~TComponent();
    }
};
