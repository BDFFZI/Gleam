#pragma once
#include <assert.h>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <ranges>
#include <stduuid/uuid.h>

#include "GleamUtility/Runtime/md5.h"
#include "FieldInfoTransferrer.h"
#include "FieldDataTransferrer.h"

namespace Gleam
{
    template <typename T>
    struct Type_Raii
    {
        /**
         * shared_ptr内部通过重写虚函数（_Ref_count<T>转_Ref_count_base）来实现多态。
         * 因此支持std::shared_ptr<T>转std::shared_ptr<void>，从而在擦除类型信息的同时保留回收物体的能力。
         * 这种特征同样对其创建的std::weak<void>有效，因为它们是共享的_Ref_count_base。
         * 但直接用std::shared_ptr<void>创建是不行的，因为void没有虚表，无法调用虚析构函数，因此有必要为创建智能指针生成反射函数。
         * @param obj 
         * @return 
         */
        static std::shared_ptr<void> MakeShared(void* obj)
        {
            return std::shared_ptr<T>(static_cast<T*>(obj));
        }
        static void* Create()
        {
            return new T();
        }
        static void Destroy(void* obj)
        {
            delete static_cast<T*>(obj);
        }
        static void Construct(void* address)
        {
            new(address) T();
        }
        static void Destruct(void* address)
        {
            static_cast<T*>(address)->T::~T();
        }
        static void MoveConstruct(void* destination, void* source) requires std::is_move_constructible_v<T>
        {
            new(destination) T(std::move(*static_cast<T*>(source)));
        }
        static void Move(void* destination, void* source) requires std::is_move_assignable_v<T>
        {
            *static_cast<T*>(destination) = std::move(*static_cast<T*>(source));
        }
        static void CopyConstruct(void* destination, void* source) requires std::is_copy_constructible_v<T>
        {
            new(destination) T(*static_cast<T*>(source));
        }
        static void Copy(void* destination, void* source) requires std::is_copy_assignable_v<T>
        {
            *static_cast<T*>(destination) = *static_cast<T*>(source);
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
        static auto GetAllTypes()
        {
            return allTypes | std::views::values;
        }

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

            Type& type = allTypes[typeid(T)];

            type.index = typeid(T);
            type.id = id.value_or(uuids::uuid(MD5(type.index.name()).toArray()));
            type.size = sizeof(T);
            type.parent = parent;
            type.makeShared = Type_Raii<T>::MakeShared;
            type.create = Type_Raii<T>::Create;
            type.destroy = Type_Raii<T>::Destroy;
            type.construct = Type_Raii<T>::Construct;
            type.destruct = Type_Raii<T>::Destruct;
            if constexpr (requires() { Type_Raii<T>::MoveConstruct; })
                type.moveConstruct = Type_Raii<T>::MoveConstruct;
            if constexpr (requires() { Type_Raii<T>::Move; })
                type.move = Type_Raii<T>::Move;
            if constexpr (requires() { Type_Raii<T>::CopyConstruct; })
                type.copyConstruct = Type_Raii<T>::CopyConstruct;
            if constexpr (requires() { Type_Raii<T>::Copy; })
                type.copy = Type_Raii<T>::Copy;

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

        Type() = default;
        Type(Type&) = delete;
        Type& operator=(Type&) = delete;

        bool operator==(const Type& other) const
        {
            return id == other.id;
        }
        bool operator!=(const Type& other) const
        {
            return !(*this == other);
        }

        std::string_view GetName() const;
        std::type_index GetIndex() const;
        uuids::uuid GetID() const;
        int GetSize() const;
        std::optional<std::reference_wrapper<const Type>> GetParent() const;
        const std::vector<FieldInfo>& GetFields() const;
        bool CanMoveConstruct() const { return moveConstruct != nullptr; }
        bool CanMove() const { return move != nullptr; }
        bool CanCopyConstruct() const { return copyConstruct != nullptr; }
        bool CanCopy() const { return copy != nullptr; }

        void SetParent(std::optional<std::reference_wrapper<const Type>> parent);
        bool FindFields(std::string_view path, std::vector<FieldInfo>& result) const;

        std::shared_ptr<void> MakeShared(void* ptr) const;
        void* Create() const;
        void Destroy(void* address) const;
        void Construct(void* address) const;
        void Destruct(void* address) const;
        void MoveConstruct(void* destination, void* source) const;
        void Move(void* destination, void* source) const;
        void CopyConstruct(void* destination, void* source) const;
        void Copy(void* destination, void* source) const;

        void Serialize(FieldDataTransferrer& transferrer, void* address, bool serializeParent = true) const;

    private:
        inline static std::unordered_map<std::type_index, Type> allTypes = {}; //unordered_map扩容不会修改元素地址
        inline static std::unordered_map<uuids::uuid, const Type*> idToType = {};

        std::type_index index = typeid(void);
        uuids::uuid id = {};
        int size = 0;
        std::optional<std::reference_wrapper<const Type>> parent = std::nullopt;
        std::vector<FieldInfo> fields = {};

        std::function<std::shared_ptr<void>(void*)> makeShared = nullptr;
        std::function<void*()> create = nullptr;
        std::function<void(void*)> destroy = nullptr;
        std::function<void(void*)> construct = nullptr;
        std::function<void(void*)> destruct = nullptr;
        std::function<void(void*, void*)> moveConstruct = nullptr;
        std::function<void(void*, void*)> move = nullptr;
        std::function<void(void*, void*)> copyConstruct = nullptr;
        std::function<void(void*, void*)> copy = nullptr;

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

#define Gleam_MakeType(type) Gleam_MakeType_Inner(type,"",std::nullopt)
#define Gleam_MakeTypeWithID(type,uuidStr) Gleam_MakeType_Inner(type,uuidStr,std::nullopt)
#define Gleam_MakeTypeWithParent(type,parentType) Gleam_MakeType_Inner(type,"",::Gleam::Type::CreateOrGet<parentType>())
#define Gleam_MakeTypeWithIDParent(type,uuidStr,parentType) Gleam_MakeType_Inner(type,uuidStr,::Gleam::Type::CreateOrGet<parentType>())