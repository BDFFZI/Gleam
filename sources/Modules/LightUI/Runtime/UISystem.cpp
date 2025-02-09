#include "UISystem.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    void UISystem::Update()
    {
        UI::BeginFrame();
        SystemGroup::Update();
        UI::EndFrame();
    }
}