#pragma once
#include <type_traits>

namespace Light
{
    template <class TComponent>
    concept Component = !std::is_pointer_v<TComponent> && !std::is_reference_v<TComponent>;

    struct ComponentInfo
    {
        using Function = void(*)(std::byte*);

        std::type_index type;
        int size;
        Function constructor;
        Function destructor;
    };

    template <Component TComponent>
    struct ComponentInfoMeta
    {
        static void Constructor(std::byte* ptr)
        {
            new(ptr) TComponent();
        }
        static void Destructor(std::byte* ptr)
        {
            reinterpret_cast<TComponent*>(ptr)->~TComponent();
        }

        ComponentInfo info = {
            typeid(TComponent),
            sizeof(TComponent),
            Constructor,
            Destructor,
        };
    };
}