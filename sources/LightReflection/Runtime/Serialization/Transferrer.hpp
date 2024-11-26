#pragma once
#include <typeindex>
#include <vector>

namespace Light
{
    class Transferrer;
    template <class TValue>
    struct ValueTransferrer
    {
        static void TransferValue(Transferrer& transferrer, TValue& value);
    };

    class Transferrer
    {
    public:
        virtual ~Transferrer() = default;

        template <class TValue>
        void TransferField(const std::string_view name, TValue& value)
        {
            PushPath(name);
            TransferValue(value);
            PopPath();
        }

        template <class TValue>
        void TransferValue(TValue& value)
        {
            ValueTransferrer<TValue>::TransferValue(*this, value);
        }
        virtual void TransferValue(void* value, std::type_index type) = 0;
        virtual void PushPath(const std::string_view name)
        {
        }
        virtual void PopPath()
        {
        }
    };

    template <class TValue>
    void ValueTransferrer<TValue>::TransferValue(Transferrer& transferrer, TValue& value)
    {
        transferrer.TransferValue(&value, typeid(TValue));
    }

    template <class TContainer>
        requires requires(TContainer container) { container.resize(0);std::size(container);std::begin(container);std::end(container); }
    struct ValueTransferrer<TContainer>
    {
        static void TransferValue(Transferrer& transferrer, TContainer& container)
        {
            int size = static_cast<int>(std::size(container));
            transferrer.TransferField("size", size);
            container.resize(size);

            transferrer.PushPath("data");
            for (auto it = std::begin(container); it != std::end(container); ++it)
                transferrer.TransferValue(*it);
            transferrer.PopPath();
        }
    };
    template <>
    struct ValueTransferrer<std::vector<bool>>
    {
        static void TransferValue(Transferrer& transferrer, std::vector<bool>& value)
        {
            int size = static_cast<int>(std::size(value));
            transferrer.TransferField("size", size);
            value.resize(size);

            transferrer.PushPath("data");
            for (auto it : value)
            {
                bool element = it;
                transferrer.TransferValue(element);
                it = element;
            }
            transferrer.PopPath();
        }
    };
    template <>
    struct ValueTransferrer<std::vector<std::byte>>
    {
        static void TransferValue(Transferrer& transferrer, std::vector<std::byte>& value)
        {
            transferrer.TransferValue(&value, typeid(std::vector<std::byte>));
        }
    };
    template <>
    struct ValueTransferrer<std::string>
    {
        static void TransferValue(Transferrer& transferrer, std::string& value)
        {
            transferrer.TransferValue(&value, typeid(std::string));
        }
    };
}
