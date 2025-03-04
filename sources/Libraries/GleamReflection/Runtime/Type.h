#pragma once
#include <assert.h>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <stduuid/uuid.h>

#include "GleamUtility/Runtime/md5.h"
#include "FieldInfoTransferrer.h"
#include "FieldDataTransferrer.h"

namespace Gleam
{
    template <typename T>
    struct Type_Raii
    {
        static void Construct(void* address)
        {
            new(address) T();
        }
        static void Destruct(void* address)
        {
            static_cast<T*>(address)->T::~T();
        }
        static void MoveConstruct(void* source, void* destination)
        {
            new(destination) T(std::move(*static_cast<T*>(source)));
        }
        static void Move(void* source, void* destination)
        {
            *static_cast<T*>(destination) = std::move(*static_cast<T*>(source));
        }
    };

    /**
     * 运行时类型信息。
     * Type是RTTI的增强版，实现了包括RAII，序列化，反射等功能。
     * Type可用uuid访问，uuid是为了解决type_index无法存储的问题。
     */
    class Type
    {
    public:
        /**
         * 基于模板、type_info创建Type，并建立type_index到Type的索引
         * @tparam T 
         * @param id 
         * @param parent 
         * @return 
         */
        template <typename T>
        static Type& Create(const std::optional<uuids::uuid> id = std::nullopt, const std::optional<std::reference_wrapper<const Type>> parent = std::nullopt)
        {
            assert(!allTypes.contains(typeid(T)) && "类型已创建！");
            assert(!id.has_value() || !allTypes.contains(typeid(T)) && "编号已占用！");

            Type& type = allTypes.emplace(typeid(T), Type{}).first->second;

            type.index = typeid(T);
            type.id = id.value_or(uuids::uuid(MD5(type.index.name()).toArray()));
            type.size = sizeof(T);
            type.parent = parent;
            type.construct = Type_Raii<T>::Construct;
            type.destruct = Type_Raii<T>::Destruct;
            type.moveConstruct = Type_Raii<T>::MoveConstruct;
            type.move = Type_Raii<T>::Move;

            //利用字段类型传输器获取成员字段信息
            {
                std::byte instance[sizeof(T)];
                type.construct(instance); //构造一个实例
                FieldInfoTransferrer memberTransferrer = {instance};
                TransferObjectField<FieldInfoTransferrer, T>::Invoke(memberTransferrer, *reinterpret_cast<T*>(instance));
                type.fields.swap(memberTransferrer.GetResult()); //获取成员信息
            }

            //序列化
            type.serialize = [](FieldDataTransferrer& serializer, void* ptr)
            {
                TransferObjectField<FieldDataTransferrer, T>::Invoke(serializer, *static_cast<T*>(ptr));
            };

            idToType.emplace(type.id, &type);
            return type;
        }
        /**
         * 当Type不存在时，会自动创建一个默认Type。
         * 相比显式创建的Type，默认Type通常会缺少固定ID、序列化、父信息等功能；但构造函数，内存大小是可用的。
         * @return 
         */
        template <class T>
        static Type& CreateOrGet()
        {
            if (allTypes.contains(typeid(T)))
                return allTypes.at(typeid(T));
            return Create<T>();
        }

        static std::optional<std::reference_wrapper<const Type>> GetType(std::type_index typeIndex);
        static std::optional<std::reference_wrapper<const Type>> GetType(uuids::uuid typeID);

        std::string_view GetName() const;
        std::type_index GetIndex() const;
        uuids::uuid GetID() const;
        int GetSize() const;
        std::optional<std::reference_wrapper<const Type>> GetParent() const;
        const std::vector<FieldInfo>& GetFields() const;

        void SetParent(std::optional<std::reference_wrapper<const Type>> parent);

        void Construct(void* address) const;
        void Destruct(void* address) const;
        void MoveConstruct(void* source, void* destination) const;
        void Move(void* source, void* destination) const;
        void Serialize(FieldDataTransferrer& transferrer, void* address, bool serializeParent = true) const;

    private:
        inline static std::unordered_map<std::type_index, Type> allTypes = {}; //unordered_map扩容不会修改元素地址
        inline static std::unordered_map<uuids::uuid, const Type*> idToType = {};

        std::type_index index = typeid(void);
        uuids::uuid id = {};
        int size = 0;
        std::optional<std::reference_wrapper<const Type>> parent = std::nullopt;
        std::vector<FieldInfo> fields = {};
        std::function<void(void*)> construct = nullptr;
        std::function<void(void*)> destruct = nullptr;
        std::function<void(void*, void*)> moveConstruct = nullptr;
        std::function<void(void*, void*)> move = nullptr;
        std::function<void(FieldDataTransferrer&, void*)> serialize = nullptr;
    };
}

#define Gleam_MakeType_Friend \
template <::Gleam::FieldTransferrer TFieldTransferrer,typename T>\
friend struct ::Gleam::TransferObjectField;\
template <typename T>\
friend struct ::Gleam::Type_Raii;

#define Gleam_MakeType_Inner(type,uuidStr,parent,...)\
inline const ::Gleam::Type& type##Type = ::Gleam::Type::Create<type>(\
uuids::uuid::from_string(uuidStr),\
parent,\
__VA_ARGS__);\
template <::Gleam::FieldTransferrer TFieldTransferrer>\
struct ::Gleam::TransferObjectField<TFieldTransferrer,type>\
{\
static void Invoke(TFieldTransferrer& transferrer, type& value);\
};\
template <::Gleam::FieldTransferrer TFieldTransferrer>\
void ::Gleam::TransferObjectField<TFieldTransferrer,type>::Invoke(TFieldTransferrer& transferrer, type& value)

#define Gleam_MakeType_AddField(field)\
transferrer.TransferField(#field, value.field)

#define Gleam_MakeType(type,uuidStr) Gleam_MakeType_Inner(type,uuidStr,std::nullopt)
#define Gleam_MakeType2(type,uuidStr,parent) Gleam_MakeType_Inner(type,uuidStr,parent)