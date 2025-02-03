#include "InspectorWindow.h"

#include "EditorUI/EditorUISerializer.h"
#include "LightECS/Runtime/World.h"
#include "LightReflection/Runtime/Type.h"
#include "LightReflection/Runtime/Transferrer/DataTransferrer.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    void InspectorWindow::Show(const Entity target)
    {
        InspectorWindow* inspectorWindow = new InspectorWindow();
        inspectorWindow->target = target;
        World::AddSystem(inspectorWindow);
    }
    void InspectorWindow::Stop()
    {
        if (this != Light::InspectorWindow)
            delete this;
    }
    void InspectorWindow::Update()
    {
        if (this == Light::InspectorWindow)
            ImGui::Begin("InspectorWindow");
        else
        {
            //非默认检视窗口，支持多窗口和关闭功能
            bool isOpen = true;
            ImGui::Begin(std::format("InspectorWindow {}", static_cast<uint32_t>(target)).c_str(), this == Light::InspectorWindow ? nullptr : &isOpen);
            if (isOpen == false)
                World::RemoveSystem(this);
        }

        if (World::HasEntity(target))
        {
            EntityInfo entityInfo = World::GetEntityInfo(target);
            const Archetype& archetype = *entityInfo.archetype;
            //绘制实体信息
            ImGui::Text("Entity:%i", static_cast<int>(target));
            //绘制组件
            for (int i = 1; i < archetype.GetComponentCount(); ++i)
            {
                const ComponentInfo& componentInfo = archetype.GetComponentInfo(i);

                std::type_index componentType = componentInfo.type;
                const char* componentName = componentInfo.type.name();
                int componentOffset = archetype.GetComponentOffset(i);
                void* component = entityInfo.components + componentOffset;
                //绘制组件标题
                ImGui::SeparatorText(componentName);
                //绘制组件内容
                Type* type = Type::GetType(componentType);
                if (type != nullptr)
                {
                    EditorUISerializer editorUiSerializer;
                    type->serialize(editorUiSerializer, component);
                }
            }
        }

        ImGui::End();
    }
}