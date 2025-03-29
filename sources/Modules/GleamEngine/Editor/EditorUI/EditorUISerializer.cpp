#include "EditorUISerializer.h"

#include "EditorUI.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamMath/Runtime/LinearAlgebra/Matrix.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    EditorUISerializer::EditorUISerializer(std::string_view rootName)
    {
        nodePaths.emplace_back(rootName);
    }
    void EditorUISerializer::PushNode(const std::optional<std::string_view> name, const DataType dataType)
    {
        nodePaths.push_back(name.has_value() ? name->data() : nodeTypes.back() == DataType::Field ? nodePaths.back() : "");
        nodeTypes.push_back(dataType);
        nodeFolds.push_back(nodeFolds.back());
        nodeIndices.push_back(0);
        nodeIndices[nodeIndices.size() - 2]++; //推送节点，父节点元素+1

        //不需要考虑Array的结构UI，因为Serializer会将Array放在Class结构中传输，故统一只要考虑Class的结构UI即可
        if (dataType == DataType::Class)
        {
            if (nodeFolds.back()) //当继承父节点的折叠信息是“没有折叠”时才可以渲染UI
                nodeFolds.back() = ImGui::TreeNode(GetNodeName().c_str());
        }
    }
    void EditorUISerializer::PopNode()
    {
        if (nodeTypes.back() == DataType::Array)
        {
            TransferBufferedArray(); //根据缓存的数组元素信息构建数组UI（为了实现将基于数组的float3等类型改为字段显示方式）
            arrayBuffer.clear();
            arrayType = 0;
        }
        if (nodeTypes.back() == DataType::Class)
        {
            if (nodeFolds.back())
                ImGui::TreePop();
        }

        nodePaths.pop_back();
        nodeTypes.pop_back();
        nodeFolds.pop_back();
        nodeIndices.pop_back();
    }

    void EditorUISerializer::Transfer(int32_t& value)
    {
        if (nodeFolds.back() == false)
            return;

        if (nodeTypes.back() == DataType::Array)
        {
            //基元数组内的元素改用缓存实现
            arrayBuffer.push_back(&value);
            arrayType = 2;
            return;
        }

        PreTransferNode();
        if (GetNodeName() == "value")
            printf("");
        if (ImGui::DragInt(GetNodeName().c_str(), &value, dragSpeed))
            printf("");
    }
    void EditorUISerializer::Transfer(int64_t& value)
    {
        if (nodeFolds.back() == false)
            return;

        PreTransferNode();
        int intValue = static_cast<int>(value);
        ImGui::DragInt(GetNodeName().c_str(), &intValue, dragSpeed);
        value = intValue;
    }
    void EditorUISerializer::Transfer(float& value)
    {
        if (nodeFolds.back() == false)
            return;

        if (nodeTypes.back() == DataType::Array)
        {
            //基元数组内的元素改用缓存实现
            arrayBuffer.push_back(&value);
            arrayType = 1;
            return;
        }

        PreTransferNode();
        ImGui::DragFloat(GetNodeName().c_str(), &value, dragSpeed);
    }
    void EditorUISerializer::Transfer(double& value)
    {
        if (nodeFolds.back() == false)
            return;

        PreTransferNode();
        float floatValue = static_cast<float>(value);
        ImGui::DragFloat(GetNodeName().c_str(), &floatValue, dragSpeed);
        value = floatValue;
    }
    void EditorUISerializer::Transfer(std::string& value)
    {
        if (nodeFolds.back() == false)
            return;

        PreTransferNode();
        if (value.size() == value.capacity()) //字符已满，需要扩容
            value.reserve(value.capacity() * 2);
        //string的字符数不计算'\0'，所以比实际缓冲区偏小，故+1。
        if (ImGui::InputText(GetNodeName().c_str(), value.data(), value.capacity() + 1))
            value.resize(strlen(value.data()));
    }
    void EditorUISerializer::Transfer(std::vector<std::byte>& value)
    {
        if (nodeFolds.back() == false)
            return;

        PreTransferNode();
        ImGui::LabelText(GetNodeName().c_str(), "binary data");
    }
    void EditorUISerializer::FallbackTransfer(void* value, const std::type_index typeIndex)
    {
        if (nodeFolds.back() == false)
            return;

        PreTransferNode();
        if (typeIndex == typeid(Entity))
        {
            EditorUI::DrawEntityField(*static_cast<Entity*>(value));
        }
        else if (typeIndex == typeid(Archetype))
        {
            ImGui::SeparatorText("Archetype");
            std::string str = to_string(*static_cast<Archetype*>(value));
            ImGui::Text(str.data());
        }
        else if (auto optionalType = Type::GetType(typeIndex); optionalType.has_value())
        {
            const Type& type = optionalType.value();
            PushNode(std::nullopt, DataType::Class);
            type.Serialize(*this, value);
            PopNode();
        }
        else
        {
            ImGui::Text("unknown type <%s>", typeIndex.name());
        }
    }
    void EditorUISerializer::FallbackTransferPtr(std::weak_ptr<void>& value, const std::type_index objectTypeIndex)
    {
        //内容和检视
        if (ImGui::Button(std::format("Ptr:{}", reinterpret_cast<uintptr_t>(value.lock().get())).c_str()))
            GlobalInspectorWindow.SetTarget(InspectorTarget{value.lock(), objectTypeIndex});
        //拖拽
        EditorUI::SetDragDropObject(value.lock(), objectTypeIndex);
        if (std::shared_ptr<void> dragging = EditorUI::GetDragDropObject(objectTypeIndex))
            value = dragging;
        //清空按钮
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
            value.reset();
        //名称
        ImGui::SameLine();
        ImGui::Text(GetNodeName().c_str());
    }

    std::string EditorUISerializer::GetElementName(size_t index)
    {
        return std::format("({})##{}", index, nodePaths.back());
    }
    std::string EditorUISerializer::GetNodeName(const int layerOffset)
    {
        std::string nodeName = nodePaths[nodePaths.size() - 1 + layerOffset];
        DataType nodeType = nodeTypes[nodeTypes.size() - 1 + layerOffset];
        DataType parentNodeType = nodeTypes[nodeTypes.size() - 2 + layerOffset];

        if (nodeType == DataType::Array) //数组内的基本元素（基本元素构成的数组）
            return GetElementName(nodeIndices[nodeTypes.size() - 1 + layerOffset] - 1); //数组内元素显示为序号
        if (parentNodeType == DataType::Array && nodeName.empty()) //数组内的非基本元素（复合元素（如类）构成的数组）
            return magic_enum::enum_name(nodeTypes.back()).data()
                + GetElementName(nodeIndices[nodeTypes.size() - 2 + layerOffset] - 1);

        return nodeName;
    }
    void EditorUISerializer::PreTransferNode()
    {
        nodeIndices.back()++;
    }

    void EditorUISerializer::TransferBufferedArray()
    {
        if (nodeFolds.back() == false)
            return;
        if (arrayType == 0)
            return; //未采用缓冲数组功能，元素已各自绘制，跳过

        PreTransferNode();
        if (arrayType == 1)
        {
            if (arrayBuffer.size() == 1)
                ImGui::DragFloat(GetNodeName(-1).c_str(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
            else if (arrayBuffer.size() == 2)
                ImGui::DragFloat2(GetNodeName(-1).c_str(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
            else if (arrayBuffer.size() == 3)
                ImGui::DragFloat3(GetNodeName(-1).c_str(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
            else if (arrayBuffer.size() == 4)
                ImGui::DragFloat4(GetNodeName(-1).c_str(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
            else if (arrayBuffer.size() == 16)
                UI::DragFloat4x4(GetNodeName(-1).c_str(), static_cast<float4x4*>(arrayBuffer[0]), dragSpeed);
            else
            {
                if (ImGui::TreeNode(GetNodeName(-1).c_str()))
                {
                    for (size_t i = 0; i < arrayBuffer.size(); i++)
                        ImGui::DragFloat(GetElementName(i).c_str(), static_cast<float*>(arrayBuffer[i]), dragSpeed);
                    ImGui::TreePop();
                }
            }
        }

        if (arrayType == 2)
        {
            if (arrayBuffer.size() == 1)
                ImGui::DragInt(GetNodeName(-1).c_str(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
            else if (arrayBuffer.size() == 2)
                ImGui::DragInt2(GetNodeName(-1).c_str(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
            else if (arrayBuffer.size() == 3)
                ImGui::DragInt3(GetNodeName(-1).c_str(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
            else if (arrayBuffer.size() == 4)
                ImGui::DragInt4(GetNodeName(-1).c_str(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
            else
            {
                if (ImGui::TreeNode(GetNodeName(-1).c_str()))
                {
                    for (size_t i = 0; i < arrayBuffer.size(); i++)
                        ImGui::DragInt(GetElementName(i).c_str(), static_cast<int*>(arrayBuffer[i]), dragSpeed);
                    ImGui::TreePop();
                }
            }
        }
    }
}