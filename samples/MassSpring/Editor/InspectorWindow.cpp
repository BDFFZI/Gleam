#include "InspectorWindow.h"

#include <imgui.h>

#include "LightECS/Runtime/Entity.hpp"
#include "LightECS/Runtime/World.h"

void InspectorWindow::Update()
{
    ImGui::Begin("InspectorWindow");

    if (inspectingTarget != nullptr)
    {
        if (inspectingMethod != nullptr)
            inspectingMethod();
        else
        {
            //默认检视方法    
        }
    }

    ImGui::End();
}
void InspectorWindow::InspectEntity()
{
    Entity entity = *static_cast<Entity*>(inspectingTarget);
    EntityInfo entityInfo = World::GetEntityInfo(entity);
    const Archetype& archetype = *entityInfo.archetype;

    for (int i = 0; i < archetype.componentCount; ++i)
    {
        const char* componentName = archetype.componentTypes[i].name();
        int componentOffset = archetype.componentOffsets[i];
        void* component = static_cast<std::byte*>(inspectingTarget) + componentOffset;

        ImGui::SeparatorText(componentName);
        // ImGui::Text("Address:%i", component);
    }
}
