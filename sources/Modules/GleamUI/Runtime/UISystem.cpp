#include "UISystem.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    void UISystem::Start()
    {
        presentationSystem = &World::GetCurrentWorld().GetSystemAllocator().GetSystem<PresentationSystem>();
    }
    void UISystem::Update()
    {
        UI::BeginFrame();
        SystemGroup::Update();
        UI::EndFrame(presentationSystem->GetPresentGCommandBuffer());
    }
}