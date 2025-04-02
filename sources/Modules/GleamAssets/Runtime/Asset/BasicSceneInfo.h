#pragma once
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    /**
     * 场景基础信息的资源包装器，用于实现场景部分信息的持久化。
     *
     * 具体包装了场景的基本属性和托管系统的信息，托管的实体数据则由 @c PersistentEntity 包装，从而实现跨场景实体引用，为后续的预制体功能做准备。
     */
    struct BasicSceneInfo
    {
        std::string name;
        std::vector<uuids::uuid> systems;
    };
    Gleam_MakeTypeWithID(BasicSceneInfo, "3CA95E07-FCD9-4DCE-ABE3-6115152EA9D7")
    {
        Gleam_MakeType_AddField(name);
        Gleam_MakeType_AddField(systems);
    }
}