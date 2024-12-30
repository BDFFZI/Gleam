#include "InspectorWindow.h"

#include "EditorUIUtility.h"
#include "LightEngine/Runtime/Reflection/Type.hpp"
#include "LightEngine/Runtime/Reflection/Serialization/Serializer.hpp"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    class EditorUISerializer : public Serializer
    {
    public:
        void Transfer(void* value, const std::type_index type) override
        {
#define MakeTransfer(targetType,drawFunc,valueType,...)\
    if (type == typeid(targetType)) {ImGui::drawFunc(path.top(),static_cast<##valueType##*>(value),__VA_ARGS__);return;}
            MakeTransfer(int, DragInt, int)
            MakeTransfer(float, DragFloat, float, 0.1f)
            MakeTransfer(float2, DragFloat2, float, 0.1f)
            MakeTransfer(float3, DragFloat3, float, 0.1f)
            MakeTransfer(float4, DragFloat4, float, 0.1f)
#undef MakeTransfer

            if (type == typeid(Entity))
            {
                EditorUIUtility::DrawEntityButton(*static_cast<Entity*>(value));
                return;
            }

            ImGui::Text(path.top());
        }
        void PushPath(const char* name) override
        {
            path.push(name);
        }
        void PopPath() override
        {
            path.pop();
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
                auto result = Type::indexToType.find(componentType);
                if (result != Type::indexToType.end())
                {
                    EditorUISerializer editorUiSerializer;
                    Type* type = result->second;
                    type->serialize(editorUiSerializer, component);
                }
            }
        }

        ImGui::End();
    }
}
