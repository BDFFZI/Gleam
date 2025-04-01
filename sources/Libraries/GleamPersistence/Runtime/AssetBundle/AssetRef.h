#pragma once
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    /**
     * 指针持久化包装器。
     *
     * 指针或类似的引用概念必须转换为AssetRef才可以被持久化。
     *
     * 1. 在运行时，用户可以使用内存地址（指针）引用对象，但持久化后内存地址不再有效，因此需要改为对资源包内资源的引用。
     * 2. 资源最终一定是存放在资源包内的，利用资源包ID和资源包内的资源槽位ID，即可确定一个资源包。
     * 3. 资源允许被移动，因此资源引用信息可能变化，具体AssetBundle的API会自动处理这些。
     */
    struct AssetRef
    {
        uuids::uuid assetBundleID;
        int assetID;

        bool operator==(const AssetRef& other) const;
    };

    Gleam_MakeTypeWithID(AssetRef, "5760DBA6-5D94-425F-A475-622421A7E84E")
    {
        Gleam_MakeType_AddField(assetBundleID);
        Gleam_MakeType_AddField(assetID);
    }

    std::string to_string(const AssetRef& value);
}

template <>
struct std::hash<Gleam::AssetRef>
{
    size_t operator()(const Gleam::AssetRef& value) const noexcept;
};