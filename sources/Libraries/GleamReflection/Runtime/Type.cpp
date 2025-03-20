#include "Type.h"
#include <ranges>

#include "GleamUtility/Runtime/String.h"

namespace Gleam
{
    std::optional<std::reference_wrapper<const Type>> Type::GetType(const std::type_index typeIndex)
    {
        if (allTypes.contains(typeIndex) == false)
            return std::nullopt;
        return allTypes.at(typeIndex);
    }
    std::optional<std::reference_wrapper<const Type>> Type::GetType(const uuids::uuid typeID)
    {
        if (idToType.contains(typeID) == false)
            return std::nullopt;
        return *idToType.at(typeID);
    }

    std::string_view Type::GetName() const
    {
        return index.name();
    }
    std::type_index Type::GetIndex() const
    {
        return index;
    }
    uuids::uuid Type::GetID() const
    {
        return id;
    }
    int Type::GetSize() const
    {
        return size;
    }
    std::optional<std::reference_wrapper<const Type>> Type::GetParent() const
    {
        return parent;
    }
    const std::vector<FieldInfo>& Type::GetFields() const
    {
        return fields;
    }

    void Type::SetParent(const std::optional<std::reference_wrapper<const Type>> parent)
    {
        this->parent = parent;
    }
    bool Type::FindFields(const std::string_view path, std::vector<FieldInfo>& result) const
    {
        result.clear();
        std::vector<std::string> propertyNames = {};
        String::Split(path, ".", propertyNames);

        const Type* currentType = this;
        for (auto it = propertyNames.begin(); it != propertyNames.end(); ++it)
        {
            //获取同名字段
            const std::string& name = *it;
            auto fieldIt = std::ranges::find_if(currentType->GetFields(), [&name](const FieldInfo& field)
            {
                return field.name == name;
            });
            //没有同名字段，返回false
            if (fieldIt == currentType->GetFields().end())
                return false;
            //获取到同名字段，添加到结果
            const FieldInfo& field = *fieldIt;
            result.push_back(field);
            //路径结束，说明字段搜索完成
            if (it + 1 == propertyNames.end())
                return true;
            //搜索需要继续深入，但该字段未注册反射，不可知其内部结构，返回false
            auto optionalType = GetType(field.type);
            if (!optionalType.has_value())
                return false;
            //进入下一次迭代
            currentType = &optionalType.value().get();
        }

        return false;
    }

    void* Type::Create() const
    {
        return create();
    }
    void Type::Destroy(void* address) const
    {
        destroy(address);
    }
    void Type::Construct(void* address) const
    {
        construct(address);
    }
    void Type::Destruct(void* address) const
    {
        destruct(address);
    }
    void Type::MoveConstruct(void* destination, void* source) const
    {
        moveConstruct(destination, source);
    }
    void Type::Move(void* destination, void* source) const
    {
        move(destination, source);
    }
    void Type::CopyConstruct(void* destination, void* source) const
    {
        copyConstruct(destination, source);
    }
    void Type::Copy(void* destination, void* source) const
    {
        copy(destination, source);
    }

    void Type::Serialize(FieldDataTransferrer& transferrer, void* address, const bool serializeParent) const
    {
        if (serializeParent && parent.has_value())
            parent.value().get().serialize(transferrer, address);
        serialize(transferrer, address);
    }
}