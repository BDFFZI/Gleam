#pragma once
#include <typeindex>
#include <vector>

namespace Light
{
    class Serializer;
    template <class TValue>
    struct SerializerOperator
    {
        static void Transfer(Serializer& serializer, TValue& value);
    };

    class Serializer
    {
    public:
        virtual ~Serializer() = default;

        template <class TValue>
        void TransferField(const std::string_view name, TValue& value)
        {
            PushPath(name);
            Transfer(value);
            PopPath();
        }
        template <class TValue>
        void Transfer(TValue& value)
        {
            SerializerOperator<TValue>::Transfer(*this, value);
        }

        virtual void Transfer(void* value, std::type_index type) = 0;
        virtual void PushPath(const std::string_view name)
        {
        }
        virtual void PopPath()
        {
        }
    };

    template <class TValue>
    void SerializerOperator<TValue>::Transfer(Serializer& serializer, TValue& value)
    {
        serializer.Transfer(&value, typeid(TValue));
    }

    template <class TContainer>
        requires requires(TContainer container) { container.resize(0);std::size(container);std::begin(container);std::end(container); }
    struct SerializerOperator<TContainer>
    {
        static void Transfer(Serializer& serializer, TContainer& value)
        {
            int size = static_cast<int>(std::size(value));
            serializer.TransferField("size", size);
            value.resize(size);

            serializer.PushPath("data");
            for (auto it = std::begin(value); it != std::end(value); ++it)
                serializer.Transfer(*it);
            serializer.PopPath();
        }
    };
    template <>
    struct SerializerOperator<std::vector<bool>>
    {
        static void Transfer(Serializer& serializer, std::vector<bool>& value)
        {
            int size = static_cast<int>(std::size(value));
            serializer.TransferField("size", size);
            value.resize(size);

            serializer.PushPath("data");
            for (auto it : value)
            {
                bool element = it;
                serializer.Transfer(element);
                it = element;
            }
            serializer.PopPath();
        }
    };
    template <>
    struct SerializerOperator<std::vector<std::byte>>
    {
        static void Transfer(Serializer& serializer, std::vector<std::byte>& value)
        {
            serializer.Transfer(&value, typeid(std::vector<std::byte>));
        }
    };
    template <>
    struct SerializerOperator<std::string>
    {
        static void Transfer(Serializer& serializer, std::string& value)
        {
            serializer.Transfer(&value, typeid(std::string));
        }
    };
}
