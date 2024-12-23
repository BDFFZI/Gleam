#include "EditorUIUtility.h"
#include "LightUI/Runtime/UI.h"
#include "InspectorWindow.h"
#include "LightEngine/Runtime/ECS/World.h"
#include "LightEngine/Runtime/ECS/_Concept.hpp"
#include "LightEngine/Runtime/Reflection/Type.hpp"

namespace Light
{
    void EditorUIUtility::DrawEntityButton(Entity entity)
    {
        if (ImGui::Button(std::format("Entity:{}", static_cast<uint32_t>(entity)).c_str()))
        {
            InspectorWindow.target = entity;
        }
    }
    void EditorUIUtility::DrawSystemGroup(SystemGroup& systemGroup)
    {
        if (ImGui::TreeNode(typeid(systemGroup).name()))
        {
            for (const auto subSystem : systemGroup.subSystemUpdateQueue)
            {
                if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                    DrawSystemGroup(*subSystemGroup);
                else
                    ImGui::Text(typeid(*subSystem).name());
            }

            ImGui::TreePop();
        }
    }
}
