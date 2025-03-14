#pragma once

#include "EntityAsset.h"

namespace Gleam
{
    /**
     *  将Entity引用包装成资源引用进行传输，从而使Entity引用可以被持久化
     */
    template <>
    struct FieldDataTransferrer_Transfer<Entity>
    {
        static void Invoke(FieldDataTransferrer& serializer, Entity& value)
        {
            //将实体引用视作AssetRef进行传输
            if (
                dynamic_cast<Serializer*>(&serializer) ||
                dynamic_cast<PointerSerializer*>(&serializer) ||
                dynamic_cast<AssetRefStatistician*>(&serializer)
            )
            {
                //将实体包装成指针。序列化引用，需要一个在两次序列化间地址不变的指针来索引资源地址和接收对象引用
                static std::unordered_map<void*, EntityAsset*> tempPointerMapping = {};
                EntityAsset*& ptr = tempPointerMapping[&value];

                auto optionalEntityAsset = EntityAsset::GetEntityAsset(value);
                ptr = optionalEntityAsset.has_value() ? &optionalEntityAsset.value().get() : nullptr;
                serializer.Transfer(ptr);
                value = ptr != nullptr ? ptr->GetEntity() : Entity::Null;
            }
            //统计未托管的可持久化实体
            else if (ObjectRefStatistician* statistician = dynamic_cast<ObjectRefStatistician*>(&serializer))
            {
                if (value == Entity::Null || EntityAsset::GetEntityAsset(value).has_value())
                    return;

                statistician->dependencies.emplace_back(EntityAsset{value, false});
            }
            //默认传输方式
            else
            {
                serializer.Transfer(&value, typeid(value));
            }
        }
    };
}