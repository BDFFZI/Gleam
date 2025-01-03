#pragma once
#include <typeindex>
#include <vector>
#include <string>

#include "../Transferrer.hpp"

namespace Light
{
    class Serializer;
    template <class TValue>
    struct SerializerTransfer
    {
        static void Invoke(Serializer& serializer, const char* name, TValue& value);
    };
    
    class Serializer
    {
    public:
        enum class NodeType:uint8_t
        {
            Field, Array, Class
        };

        virtual ~Serializer() = default;

        template <class TValue>
        void TransferField(const char* name, TValue& value)
        {
            SerializerTransfer<TValue>::Invoke(*this, name, value);
        }

    protected:
        template <class TValue>
        friend struct SerializerTransfer;

        template <class TValue>
        void Transfer(TValue& value)
        {
            Transfer(&value, typeid(TValue));
        }
        virtual void Transfer(void* value, std::type_index type) = 0;
        virtual void PushNode(const char* name, NodeType nodeType) =0;
        virtual void PopNode() =0;

#define Light_TransferPart_Inner(function,valueType) if (type == typeid(valueType)){function(*static_cast<valueType##*>(value));return;}
#define Light_Transfer_Inner(function) \
        Light_TransferPart_Inner(function,bool)\
        Light_TransferPart_Inner(function,float)\
        Light_TransferPart_Inner(function,double)\
        Light_TransferPart_Inner(function,int32_t)\
        Light_TransferPart_Inner(function,uint32_t)\
        Light_TransferPart_Inner(function,uint64_t)\
        Light_TransferPart_Inner(function,std::string)\
        Light_TransferPart_Inner(function,std::vector<std::byte>)
    };
    static_assert(Transferrer<Serializer>);

    ///默认传输方案
    template <class TValue>
    void SerializerTransfer<TValue>::Invoke(Serializer& serializer, const char* name, TValue& value)
    {
        serializer.PushNode(name, Serializer::NodeType::Field);
        serializer.Transfer(value);
        serializer.PopNode();
    }

    ///容器传输方案
    template <class TContainer>
        requires requires(TContainer container)
        {
            std::size(container);
            std::begin(container);
            std::end(container);
        }
        && !std::is_same_v<TContainer, std::vector<std::byte>>
        && !std::is_same_v<TContainer, std::string>
    struct SerializerTransfer<TContainer>
    {
        static void Invoke(Serializer& serializer, const char* name, TContainer& value)
        {
            serializer.PushNode(name, Serializer::NodeType::Class);
            {
                int size = static_cast<int>(std::size(value));
                serializer.TransferField("size", size);
                if (requires() { value.resize(size); })
                    value.resize(size);
            }
            {
                serializer.PushNode("data", Serializer::NodeType::Array);
                for (auto it = std::begin(value); it != std::end(value); ++it)
                    serializer.Transfer(*it);
                serializer.PopNode();
            }
            serializer.PopNode();
        }
    };
    //vector<bool>特殊处理
    template <>
    struct SerializerTransfer<std::vector<bool>>
    {
        static void Invoke(Serializer& serializer, const char* name, std::vector<bool>& value)
        {
            serializer.PushNode(name, Serializer::NodeType::Class);
            {
                int size = static_cast<int>(std::size(value));
                serializer.TransferField("size", size);
                if (requires() { value.resize(size); })
                    value.resize(size);
            }
            {
                serializer.PushNode("data", Serializer::NodeType::Array);
                for (auto it : value)
                {
                    bool element = it;
                    serializer.Transfer(element);
                    it = element;
                }
                serializer.PopNode();
            }
            serializer.PopNode();
        }
    };
    //char特殊处理
    template <>
    inline void Serializer::Transfer<char>(char& value)
    {
        int intValue = static_cast<int>(value); // NOLINT(bugprone-signed-char-misuse)
        Transfer(intValue);
        value = static_cast<char>(intValue);
    }
}