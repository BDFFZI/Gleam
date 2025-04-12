#include "EditorUISerializer.h"

#include "EditorUI.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"

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
            //尝试根据缓存的数组元素信息构建数组UI（用于实现将基于数组的float3等类型改为字段显示方式）
            TransferBufferedVector();
            bufferedVectorItem.clear();
            bufferedVectorItemType = -1;
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

    void EditorUISerializer::TransferNumber(void* address, const int size, const ImGuiDataType dataType)
    {
        if (nodeFolds.back() == false)
            return;

        if (nodeTypes.back() == DataType::Array)
        {
            //基元数组内的元素改用缓存实现
            PushBufferedVectorItem(address, size, dataType);
            return;
        }

        PreTransferNode();
        ImGui::DragScalar(GetNodeName().c_str(), dataType, address, dragSpeed);
    }

    void EditorUISerializer::Transfer(bool& value)
    {
        if (nodeFolds.back() == false)
            return;

        PreTransferNode();
        ImGui::Checkbox(GetNodeName().c_str(), &value);
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
        ImGui::PushID(&value);
        {
            //内容和检视
            if (ImGui::Button(std::format("Ptr:{}", reinterpret_cast<uintptr_t>(value.lock().get())).c_str()))
                GlobalInspectorWindow->SetMajorTarget(InspectorTarget{value.lock(), objectTypeIndex});
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
        ImGui::PopID();
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

    void EditorUISerializer::PushBufferedVectorItem(void* itemAddress, const int itemSize, const ImGuiDataType itemType)
    {
        bufferedVectorItem.emplace_back(itemAddress);
        bufferedVectorItemType = itemType;
    }
    void EditorUISerializer::TransferBufferedVector()
    {
        if (nodeFolds.back() == false)
            return;
        if (bufferedVectorItemType == -1)
            return; //未采用缓冲数组功能，元素已各自绘制，跳过

        PreTransferNode();

        int elementCount = static_cast<int>(bufferedVectorItem.size());
        size_t elementSize = ImGui::DataTypeGetInfo(bufferedVectorItemType)->Size;

        if (elementCount <= 4 || elementCount == 9 || elementCount == 16)
        {
            static std::vector<std::byte> vectorBuffer = {};
            //将缓存的向量元素拼成真正的向量
            vectorBuffer.resize(elementCount * elementSize);
            for (int i = 0; i < elementCount; ++i)
                memcpy(vectorBuffer.data() + i * elementSize, bufferedVectorItem[i], elementSize);
            //绘制
            if (elementCount <= 4) //向量
                ImGui::DragScalarN(GetNodeName(-1).c_str(), bufferedVectorItemType, vectorBuffer.data(), elementCount, dragSpeed);
            else if (elementCount == 9) //3x3矩阵
                UI::DragScalarMatrix(GetNodeName(-1).c_str(), bufferedVectorItemType, vectorBuffer.data(), 3, 3, dragSpeed);
            else if (elementCount == 16) //4x4矩阵
                UI::DragScalarMatrix(GetNodeName(-1).c_str(), bufferedVectorItemType, vectorBuffer.data(), 4, 4, dragSpeed);
            //将向量写回到各个元素原位置
            for (int i = 0; i < elementCount; ++i)
                memcpy(bufferedVectorItem[i], vectorBuffer.data() + i * elementSize, elementSize);
        }
        else
        {
            //按正常数组绘制
            if (ImGui::TreeNode(GetNodeName(-1).c_str()))
            {
                for (size_t i = 0; i < bufferedVectorItem.size(); i++)
                    ImGui::DragScalar(GetElementName(i).c_str(), bufferedVectorItemType, bufferedVectorItem[i], dragSpeed);
                ImGui::TreePop();
            }
        }
    }
}