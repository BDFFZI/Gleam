#include "UISystem.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    void UISystem::Update()
    {
        UI::BeginFrame();
        ISystemGroup::Update();
        UI::EndFrame(GlobalPresentationSystem->GetPresentGCommandBuffer());
    }
}