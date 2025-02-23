#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Runtime/Engine.h"

namespace Gleam
{
    inline SystemEvent DefaultUpdateOrder = SystemEvent("DefaultUpdateOrder", std::nullopt);

    /**
     * 在默认更新顺序之前执行，用于给用户提供每帧开始前的必要信息，例如外部输入，时间等，以便用户根据这些信息更新游戏数据。
     */
    class PreUpdateSystem : public SystemGroup
    {
    public:
        PreUpdateSystem(): SystemGroup(DefaultUpdateOrder, OrderRelation::Before)
        {
        }
    };
    Gleam_MakeGlobalSystem(PreUpdateSystem)

    /**
     * 在默认更新顺序之后执行，用于引擎接收处理用户更新后的游戏数据，系统将根据这些信息调整一些系统功能，例如更新矩阵，绘制。
     */
    class PostUpdateSystem : public SystemGroup
    {
    public:
        PostUpdateSystem(): SystemGroup(DefaultUpdateOrder, OrderRelation::After)
        {
        }
    };
    Gleam_MakeGlobalSystem(PostUpdateSystem)
}