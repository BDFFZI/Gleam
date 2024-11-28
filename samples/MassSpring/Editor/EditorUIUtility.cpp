#include "EditorUIUtility.h"

#include <imgui.h>
#include <typeindex>

#include "InspectorWindow.h"
#include "LightECS/Runtime/World.h"
#include "LightECS/Runtime/_Concept.hpp"
#include "LightReflection/Runtime/Type.hpp"

namespace Light
{
    void EditorUIUtility::DrawEntityButton(Entity entity)
    {
        if (ImGui::Button(std::format("Entity:{}", static_cast<uint32_t>(entity)).c_str()))
        {
            InspectorWindow::target = entity;
        }
    }
}
