#include "InspectorWindow.h"

#include "EditorUIUtility.h"
#include "LightReflection/Runtime/Type.h"
#include "LightReflection/Runtime/Transferrer/DataTransferrer.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    class EditorUISerializer : public DataTransferrer
    {
    public:
        void PushNode(const char* name, DataType dataType) override
        {
            path.push(name);
        }
        void PopNode() override
        {
            path.pop();
        }

        void Transfer(double& value) override
        {
            float floatValue = static_cast<float>(value);
            ImGui::DragFloat(path.top(), &floatValue, 0.1f);
            value = floatValue;
        }
        void Transfer(int64_t& value) override
        {
            int intValue = static_cast<int>(value);
            ImGui::DragInt(path.top(), &intValue, 0.1f);
            value = intValue;
        }
        void Transfer(std::string& value) override
        {
            if (value.size() + 1 > value.capacity())
                value.reserve(value.capacity() * 2);
            if (ImGui::InputText(path.top(), value.data(), value.capacity()))
                value.resize(strlen(value.data()));
        }
        void Transfer(std::vector<std::byte>& value) override
        {
            ImGui::LabelText(path.top(), "binary data");
        }
        void Transfer(void* value, const std::type_index type) override
        {
            if (type == typeid(Entity))
            {
                EditorUIUtility::DrawEntityButton(*static_cast<Entity*>(value));
            }
        }

        std::stack<const char*> path;
    };

    void InspectorWindow::Update()
    {
        ImGui::Begin("InspectorWindow");

        if (World::HasEntity(target))
        {
            EntityInfo entityInfo = World::GetEntityInfo(target);
            const Archetype& archetype = *entityInfo.archetype;
            //绘制实体信息
            ImGui::Text("Entity:%i", static_cast<int>(target));
            //绘制组件
            for (int i = 1; i < archetype.componentCount; ++i)
            {
                const char* componentName = archetype.componentTypes[i].name();
                int componentOffset = archetype.componentOffsets[i];
                std::type_index componentType = archetype.componentTypes[i];
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