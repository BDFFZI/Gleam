#include "EditorUISystem.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    void EditorUISystem::Update()
    {
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
        
        SystemGroup::Update();
    }
}