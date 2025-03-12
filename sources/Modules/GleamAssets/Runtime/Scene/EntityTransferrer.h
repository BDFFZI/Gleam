#pragma once
#include "EntityAsset.h"

namespace Gleam
{
    /**
     * Entity引用资源化函数
     */
    template <>
    struct FieldDataTransferrer_Transfer<Entity>
    {
        /**
         * 注意！由于实体地址会变动，故该映射表仅在反序列化时有效，序列化时应使用资源替身值。
         * 若未按正确的依赖顺序加载资源包，且场景中实体发生结构性变动，那引用将丢失或发生错乱！
         */
        inline static std::unordered_map<void*, AssetRef> pointerMapping = {};

        static void Invoke(FieldDataTransferrer& serializer, Entity& value)
        {
            //获取资源化替身
            std::optional<std::reference_wrapper<EntityAsset>> optionalAsset = EntityAsset::GetEntityAsset(value);
            EntityAsset* assetPtr = optionalAsset.has_value() ? &optionalAsset.value().get() : nullptr;
            //优先获取资源化替身的资源引用（序列化时保存），否则使用指针映射表存储的资源引用（首次反序列化结束时保存）
            AssetRef assetRef = AssetBundle::GetAssetRef(assetPtr).value_or(pointerMapping[&value]);
            assert(value == Entity::Null || !assetRef.assetBundleID.is_nil() && "引用的实体未被资源化！");
            //序列化或反序列化引用信息
            serializer.Transfer(assetRef);
            pointerMapping[&value] = assetRef; //存储指针对应的引用信息，以供第二次指针遍历时依然能拿到指针引用
            //重新定位资源地址
            assetPtr = static_cast<EntityAsset*>(AssetBundle::GetObject(assetRef).value_or(nullptr));
            value = assetPtr != nullptr ? assetPtr->GetEntity() : value;
        }
    };
}