#include "UISystem.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    void UISystem::Update()
    {
        PresentationSystem& presentationSystem = World::GetSystemAllocator().GetSystem<PresentationSystem>();

        UI::BeginFrame();
        ISystemGroup::Update();
        UI::EndFrame(presentationSystem.GetPresentGCommandBuffer());
    }
}