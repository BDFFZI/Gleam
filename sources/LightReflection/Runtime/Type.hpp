#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <stduuid/uuid.h>

#include "Serialization/Serializer.hpp"
#include "Concept.hpp"

namespace Light
{
    struct FieldInfo
    {
        std::string name;
        std::type_index type;
        std::ptrdiff_t offset;
        std::size_t size;
    };

    template <typename T>
    struct TypeConstruct
    {
        static void Invoke(void* address)
        {
            new(address) T();
        }
    };

    template <typename T,Transferrer TTransferrer>
    struct TypeTransfer
    {
        static void Invoke(TTransferrer& transferrer, T& value)
        {
            throw std::logic_error("未实现类型传输函数！");
        }
    };

    struct Type
    {
        using Construct = void (*)(void* address);
        using Serialize = void (*)(Serializer&, void*);

        inline static std::vector<std::unique_ptr<Type>> allTypes = {};
        inline static std::unordered_map<uuids::uuid, Type*> uuidToType = {};
        inline static std::unordered_map<std::type_index, Type*> indexToType = {};

        template <typename T>
        static Type& Register(const char* uuidStr)
        {
            std::unique_ptr<Type>& type = allTypes.emplace_back(new Type());
            
            uuids::uuid uuid = uuids::uuid::from_string(uuidStr).value_or(uuids::uuid());
            uuidToType.insert({uuid, type.get()});
            indexToType.insert({typeid(T), type.get()});

            type->uuid = uuid;
            type->info = &typeid(T);
            type->size = sizeof(T);
            type->construct = TypeConstruct<T>::Invoke;
            type->serialize = [](Serializer& serializer, void* ptr) { TypeTransfer<T, Serializer>::Invoke(serializer, *static_cast<T*>(ptr)); };
            type->deserialize = type->serialize;

            std::byte instance[sizeof(T)];
            type->construct(instance);
            MemberTransferrer memberTransferrer = {instance};
            TypeTransfer<T, MemberTransferrer>::Invoke(memberTransferrer, *reinterpret_cast<T*>(instance));
            type->fieldInfos.swap(memberTransferrer.GetResult());

            return *type;
        }

        uuids::uuid uuid;
        const type_info* info = nullptr;
        size_t size = 0;
        Construct construct = nullptr;
        Serialize serialize = nullptr;
        Serialize deserialize = nullptr;
        std::vector<FieldInfo> fieldInfos;

    private:
        class MemberTransferrer
        {
        public:
            MemberTransferrer(void* target)
                : targetAddress(static_cast<std::byte*>(target))
            {
            }

            std::vector<FieldInfo>& GetResult()
            {
                fieldInfos.shrink_to_fit();
                return fieldInfos;
            }

            template <class TMember>
            void TransferField(const char* name, TMember& value)
            {
                fieldInfos.emplace_back(
                    name,
                    typeid(TMember),
                    reinterpret_cast<std::byte*>(&value) - targetAddress,
                    sizeof(TMember)
                );
            }

        private:
            std::byte* targetAddress;
            std::vector<FieldInfo> fieldInfos;
        };
        static_assert(Transferrer<MemberTransferrer>);
    };

#define MakeType_AddField(field) transferrer.TransferField(#field, value.field)
#define MakeType(uuidStr,type,...)\
    inline const Light::Type& type##Type = Light::Type::Register<type>(uuidStr);\
    template <Light::Transferrer TTransferrer>\
    struct Light::TypeTransfer<type, TTransferrer>\
    {static void Invoke(TTransferrer& transferrer, type& value);};\
    template <Light::Transferrer TTransferrer>\
    void Light::TypeTransfer<type,TTransferrer>::Invoke(TTransferrer& transferrer, type& value)
}
