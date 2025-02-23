#include "UISystem.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    void UISystem::Update()
    {
        UI::BeginFrame();
        SystemGroup::Update();
        UI::EndFrame();
    }
}