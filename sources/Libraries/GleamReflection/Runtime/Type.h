#pragma once
#include <assert.h>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <stduuid/uuid.h>

#include "Transferrer/MemberTransferrer.h"
#include "Transferrer/Transferrer.h"

namespace Gleam
{
    template <typename T>
    struct Type_Construct
    {
        static void Invoke(void* address)
        {
            new(address) T();
        }
    };
    template <typename T,Transferrer TTransferrer>
    struct Type_Transfer
    {
        // 若要支持成员序列化功能，需特化下方函数
        // static void Invoke(TTransferrer& transferrer, T& value)
        // {
        // }
    };
    class DataTransferrer;
    class Type
    {
        using TypeConstruct = void (*)(void* address);
        using TypeSerialize = void (*)(DataTransferrer&, void*);

    public:
        /**
         * 辅助函数，利用模板功能快速设置Type信息。
         * 该函数不是创建Type的必要函数，Type始终存在，只是默认情况下各项值都为空，利用该函数可以快速设置它们。
         * @tparam T 
         * @param uuidStr 
         * @param parent 
         * @return 
         */
        template <typename T>
        static Type& Init(const std::string_view uuidStr, const std::optional<std::type_index> parent = std::nullopt)
        {
            static TypeSerialize parentSerialize = GetType(parent.value_or(typeid(void))).Serialize();

            Type& type = GetType(typeid(T));
            uuids::uuid uuid = uuids::uuid::from_string(uuidStr).value_or(type.uuid);
            if (uuid.is_nil() == false)
                uuidToTypeIndex.insert({uuid, typeid(T)});

            type.parent = parent;
            type.uuid = uuid;
            type.typeInfo = &typeid(T);
            type.size = sizeof(T);
            type.construct = Type_Construct<T>::Invoke;

            //根据传输器获取序列化函数
            if constexpr (requires() { Type_Transfer<T, DataTransferrer>::Invoke; }) //有自定义序列化函数
            {
                if (parentSerialize)
                    type.serialize = [](DataTransferrer& serializer, void* ptr)
                    {
                        parentSerialize(serializer, ptr);
                        Type_Transfer<T, DataTransferrer>::Invoke(serializer, *static_cast<T*>(ptr));
                    };
                else
                    type.serialize = [](DataTransferrer& serializer, void* ptr)
                    {
                        Type_Transfer<T, DataTransferrer>::Invoke(serializer, *static_cast<T*>(ptr));
                    };
            }
            else if (parentSerialize) //无自定义序列化函数，但有父类序列化函数
                type.serialize = parentSerialize;
            else //兜底使用空序列化函数
                type.serialize = [](DataTransferrer&, void*)
                {
                };

            //利用传输器获取成员信息
            if constexpr (requires() { Type_Transfer<T, MemberTransferrer>::Invoke; })
            {
                std::byte instance[sizeof(T)];
                type.construct(instance); //构造一个实例
                MemberTransferrer memberTransferrer = {instance};
                Type_Transfer<T, MemberTransferrer>::Invoke(memberTransferrer, *reinterpret_cast<T*>(instance));
                type.fieldInfos.swap(memberTransferrer.GetResult()); //获取成员信息
            }

            return type;
        }
        static Type& GetType(std::type_index typeIndex);
        static std::optional<std::reference_wrapper<Type>> GetType(std::string_view uuidStr);

        const type_info*& TypeInfo();
        std::vector<FieldInfo>& FieldInfos();
        std::optional<std::type_index>& Parent();
        size_t& Size();
        uuids::uuid& Uuid();
        TypeConstruct& Construct();
        TypeSerialize& Serialize();

        bool IsInitialized() const;

    private:
        inline static std::unordered_map<std::type_index, Type> allTypes;
        inline static std::unordered_map<uuids::uuid, std::type_index> uuidToTypeIndex = {};

        const type_info* typeInfo = nullptr;
        std::vector<FieldInfo> fieldInfos = {};
        std::optional<std::type_index> parent = std::nullopt;
        size_t size = 0;
        uuids::uuid uuid = {};
        TypeConstruct construct = nullptr;
        TypeSerialize serialize = nullptr;
    };
}

#include "Transferrer/DataTransferrer.h"

#define Gleam_MakeType_Friend \
template <typename T,Transferrer TTransferrer>\
friend struct ::Gleam::Type_Transfer;\
template <typename T>\
friend struct ::Gleam::Type_Construct;

#define Gleam_MakeType(type,uuidStr,...)\
inline const ::Gleam::Type& type##Type = ::Gleam::Type::Init<type>(uuidStr,__VA_ARGS__);\
template <Gleam::Transferrer TTransferrer>\
struct ::Gleam::Type_Transfer<type, TTransferrer>\
{static void Invoke(TTransferrer& transferrer, type& value);};\
template <Gleam::Transferrer TTransferrer>\
void ::Gleam::Type_Transfer<type,TTransferrer>::Invoke(TTransferrer& transferrer, type& value)

#define Gleam_MakeType_AddField(field)\
transferrer.TransferField(#field, value.field)