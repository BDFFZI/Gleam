#include "GizmosSystem.h"
#include "GleamRendering/Editor/Gizmos.h"

namespace Gleam
{
    void GizmosSystem::Update()
    {
        static std::vector<RendererInfo> outputRendererInfos = {};
        Gizmos::Render(outputRendererInfos);
        World::GetSystemAllocator().GetSystem<RenderingSystem>().AddRendererInfos(outputRendererInfos);
    }
    void GizmosSystem_PostProcess::Update()
    {
        //清空绘制队列
        Gizmos::Clear();
    }
}