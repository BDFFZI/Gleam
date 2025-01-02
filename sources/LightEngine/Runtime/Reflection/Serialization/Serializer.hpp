#pragma once
#include <typeindex>
#include <vector>
#include <string>
#include <format>

#include "../Transferrer.hpp"

namespace Light
{
    class Serializer;
    template <class TValue>
    struct SerializerTransfer
    {
        static void Invoke(Serializer& serializer, TValue& value);
    };

    class Serializer
    {
    public:
        virtual ~Serializer() = default;

        template <class TValue>
        void TransferField(const char* name, TValue& value)
        {
            PushPath(name);
            Transfer(value);
            PopPath();
        }
        template <class TValue>
        void Transfer(TValue& value)
        {
            SerializerTransfer<TValue>::Invoke(*this, value);
        }

    protected:
        template <class TValue>
        friend struct SerializerTransfer;

        virtual void Transfer(void* value, std::type_index type) = 0;
        virtual void PushPath(const char* name)
        {
        }
        virtual void PopPath()
        {
        }
    };
    static_assert(Transferrer<Serializer>);

    template <class TValue>
    void SerializerTransfer<TValue>::Invoke(Serializer& serializer, TValue& value)
    {
        serializer.Transfer(&value, typeid(TValue));
    }

    template <class TContainer>
        requires requires(TContainer container) { container.resize(0);std::size(container);std::begin(container);std::end(container); }
    struct SerializerTransfer<TContainer>
    {
        static void Invoke(Serializer& serializer, TContainer& value)
        {
            int size = static_cast<int>(std::size(value));
            serializer.TransferField("size", size);
            value.resize(size);

            serializer.PushPath("data");

            int itemIndex = 0;
            for (auto it = std::begin(value); it != std::end(value); ++it)
            {
                serializer.PushPath(std::format("item{}", itemIndex++).c_str());
                serializer.Transfer(*it);
                serializer.PopPath();
            }

            serializer.PopPath();
        }
    };
    template <>
    struct SerializerTransfer<std::vector<bool>>
    {
        static void Invoke(Serializer& serializer, std::vector<bool>& value)
        {
            int size = static_cast<int>(std::size(value));
            serializer.TransferField("size", size);
            value.resize(size);

            serializer.PushPath("data");

            int itemIndex = 0;
            for (auto it : value)
            {
                serializer.PushPath(std::format("item{}", itemIndex++).c_str());
                bool element = it;
                serializer.Transfer(element);
                it = element;
                serializer.PopPath();
            }

            serializer.PopPath();
        }
    };
    template <>
    struct SerializerTransfer<std::vector<std::byte>>
    {
        static void Invoke(Serializer& serializer, std::vector<std::byte>& value)
        {
            serializer.Transfer(&value, typeid(std::vector<std::byte>));
        }
    };
    template <>
    struct SerializerTransfer<std::string>
    {
        static void Invoke(Serializer& serializer, std::string& value)
        {
            serializer.Transfer(&value, typeid(std::string));
        }
    };
}