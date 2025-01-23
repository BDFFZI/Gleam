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
    class ComponentInfoMeta
    {
    public:
        static ComponentInfo GetInfo()
        {
            return {
                typeid(TComponent),
                sizeof(TComponent),
                Constructor,
                Destructor,
            };
        }
        static void Constructor(std::byte* ptr)
        {
            new(ptr) TComponent();
        }
        static void Destructor(std::byte* ptr)
        {
            reinterpret_cast<TComponent*>(ptr)->~TComponent();
        }
    };
}