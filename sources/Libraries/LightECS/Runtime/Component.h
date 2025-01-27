#pragma once
#include <type_traits>

namespace Light
{
    template <class TComponent>
    concept Component = !std::is_pointer_v<TComponent> && !std::is_reference_v<TComponent>;

    struct ComponentInfo
    {
        std::type_index type;
        int size;
        void (*constructor)(std::byte*);
        void (*destructor)(std::byte*);
        void (*moveConstructor)(std::byte*, std::byte*);
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
                MoveConstructor
            };
        }
        static void Constructor(std::byte* destination)
        {
            new(destination) TComponent();
        }
        static void Destructor(std::byte* destination)
        {
            reinterpret_cast<TComponent*>(destination)->~TComponent();
        }
        /**
         * 原本我以为直接使用memcpy就可以，但事实上很多非平凡类型（trivially_copyable）不能直接内存复制，
         * 因为它们有自己的移动或复制构造函数，且很多数据可能与旧地址存在关联，不能直接迁移。
         * 
         * 例如std::vector直接采用内存复制并不能完全迁移所有数据，导致对其的部分操作会产生异常（例如将无法创建有效的迭代器），
         * 因为它有一个名为_Mycont的堆数据会记录自身的地址，因此更换std::vector地址时必须修改该值。
         * 但显然这种解决方案非常麻烦，故还是记录下组件的移动和复制构造函数为好。
         */
        static void MoveConstructor(std::byte* source, std::byte* destination)
        {
            TComponent& component = *reinterpret_cast<TComponent*>(source);
            new(destination) TComponent(std::move(component));
        }
    };
}