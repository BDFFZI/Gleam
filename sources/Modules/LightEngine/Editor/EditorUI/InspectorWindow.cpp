#include "InspectorWindow.h"

#include "EditorUIUtility.h"
#include "LightECS/Runtime/World.h"
#include "LightReflection/Runtime/Type.h"
#include "LightReflection/Runtime/Transferrer/DataTransferrer.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    class EditorUISerializer : public DataTransferrer
    {
    public:
        void PushNode(const char* name, const DataType dataType) override
        {
            nodePath.push(dataType == DataType::Array ? nodePath.top() : name);
            nodeType.push(dataType);
            arrayBuffer.clear();
            arrayType = 0;
        }
        void PopNode() override
        {
            if (nodeType.top() == DataType::Array)
                FlushArray();

            nodePath.pop();
            nodeType.pop();
        }

        void Transfer(double& value) override
        {
            if (nodeType.top() == DataType::Array)
            {
                ImGui::Text("unsupported type");
                return;
            }

            float floatValue = static_cast<float>(value);
            Transfer(floatValue);
            value = floatValue;
        }
        void Transfer(int64_t& value) override
        {
            if (nodeType.top() == DataType::Array)
            {
                ImGui::Text("unsupported type");
                return;
            }

            int intValue = static_cast<int>(value);
            Transfer(intValue);
            value = intValue;
        }
        void Transfer(float& value) override
        {
            if (nodeType.top() == DataType::Array)
            {
                arrayBuffer.push_back(&value);
                arrayType = 1;
                return;
            }

            ImGui::DragFloat(nodePath.top(), &value, dragSpeed);
        }
        void Transfer(int32_t& value) override
        {
            if (nodeType.top() == DataType::Array)
            {
                arrayBuffer.push_back(&value);
                arrayType = 2;
                return;
            }

            ImGui::DragInt(nodePath.top(), &value, dragSpeed);
        }
        void Transfer(std::string& value) override
        {
            if (nodeType.top() == DataType::Array)
                arrayBuffer.push_back(&value);

            if (value.size() + 1 > value.capacity())
                value.reserve(value.capacity() * 2);
            if (ImGui::InputText(std::format("{}#{}", nodePath.top(), arrayBuffer.size()).c_str(), value.data(), value.capacity()))
                value.resize(strlen(value.data()));
        }
        void Transfer(std::vector<std::byte>& value) override
        {
            ImGui::LabelText(nodePath.top(), "binary data");
        }
        void Transfer(void* value, const std::type_index type) override
        {
            if (type == typeid(Entity))
            {
                EditorUIUtility::DrawEntityButton(*static_cast<Entity*>(value));
                return;
            }
            
            ImGui::Text("unknown type");
        }

    private:
        std::stack<const char*> nodePath;
        std::stack<DataType> nodeType;
        float dragSpeed = 0.1f;
        std::vector<void*> arrayBuffer;
        int arrayType = 0;

        void FlushArray()
        {
            if (arrayType == 0)
                return;

            if (arrayType == 1)
            {
                if (arrayBuffer.size() == 1)
                    ImGui::DragFloat(nodePath.top(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
                else if (arrayBuffer.size() == 2)
                    ImGui::DragFloat2(nodePath.top(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
                else if (arrayBuffer.size() == 3)
                    ImGui::DragFloat3(nodePath.top(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
                else if (arrayBuffer.size() == 4)
                    ImGui::DragFloat4(nodePath.top(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
                else
                {
                    for (size_t i = 0; i < arrayBuffer.size(); i++)
                        ImGui::DragFloat(std::format("{}#{}", nodePath.top(), i).c_str(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
                }
            }

            if (arrayType == 2)
            {
                if (arrayBuffer.size() == 1)
                    ImGui::DragInt(nodePath.top(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
                else if (arrayBuffer.size() == 2)
                    ImGui::DragInt2(nodePath.top(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
                else if (arrayBuffer.size() == 3)
                    ImGui::DragInt3(nodePath.top(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
                else if (arrayBuffer.size() == 4)
                    ImGui::DragInt4(nodePath.top(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
                else
                {
                    for (size_t i = 0; i < arrayBuffer.size(); i++)
                        ImGui::DragInt(std::format("{}#{}", nodePath.top(), i).c_str(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
                }
            }
        }
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