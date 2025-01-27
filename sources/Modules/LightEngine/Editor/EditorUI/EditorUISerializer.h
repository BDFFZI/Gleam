#pragma once
#include <iostream>

#include "EditorUIUtility.h"
#include "LightUI/Runtime/UI.h"
#include "LightReflection/Runtime/Transferrer/DataTransferrer.h"
#include "LightWindow/Runtime/Time.h"

namespace Light
{
    class EditorUISerializer : public DataTransferrer
    {
    public:
        void PushNode(const char* name, const DataType dataType) override
        {
            // nodePath.push_back(dataType == DataType::Array ? nodePath.back() : name);
            // nodeType.push_back(dataType);
            // nodeIndex.back()++;
            // nodeIndex.push_back(0);
            //
            // if (dataType == DataType::Class)
            // {
            //     nodeUnfolding.push_back(ImGui::TreeNode(name));
            // }
        }
        void PopNode() override
        {
            // DataType dataType = nodeType.back();
            //
            // nodePath.pop_back();
            // nodeType.pop_back();
            // nodeIndex.pop_back();
            //
            // if (dataType == DataType::Array)
            // {
            //     FlushArray();
            //     arrayBuffer.clear();
            //     arrayType = 0;
            // }
            // if (dataType == DataType::Class)
            // {
            //     if (nodeUnfolding.back())
            //         ImGui::TreePop();
            //     nodeUnfolding.pop_back();
            // }
        }


        void Transfer(float& value) override
        {
            // if (nodeUnfolding.back() == false)
            //     return;
            // if (nodeType.back() == DataType::Array)
            // {
            //     nodeIndex.back()++;
            //     arrayBuffer.push_back(&value);
            //     arrayType = 1;
            //     return;
            // }
            //
            // ImGui::DragFloat(GetNodeName().c_str(), &value, dragSpeed);
        }
        void Transfer(int32_t& value) override
        {
            // if (nodeUnfolding.back() == false)
            //     return;
            // if (nodeType.back() == DataType::Array)
            // {
            //     nodeIndex.back()++;
            //     arrayBuffer.push_back(&value);
            //     arrayType = 2;
            //     return;
            // }
            //
            // ImGui::DragInt(GetNodeName().c_str(), &value, dragSpeed);
        }

        void Transfer(double& value) override
        {
            // if (nodeUnfolding.back() == false)
            //     return;
            // if (nodeType.back() == DataType::Array)
            //     nodeIndex.back()++;
            //
            // float floatValue = static_cast<float>(value);
            // ImGui::DragFloat(GetNodeName().c_str(), &floatValue, dragSpeed);
            // value = floatValue;
        }
        void Transfer(int64_t& value) override
        {
            int intValue = static_cast<int>(value);
            ImGui::DragInt(std::format("value##{}", std::to_string(reinterpret_cast<uintptr_t>(&value))).c_str(), &intValue);
            if (Time->GetTime() > 3)
                value = value == 3 ? 4 : value;
            std::cout << intValue << std::endl;
            // value = 4;
            // if (nodeUnfolding.back() == false)
            //     return;
            // if (nodeType.back() == DataType::Array)
            //     nodeIndex.back()++;
            //
            // int intValue = static_cast<int>(value);
            // nodeID++;
            // ImGui::DragInt(GetNodeName().c_str(), &intValue, dragSpeed);
            // value = intValue;
        }
        void Transfer(std::string& value) override
        {
            // if (nodeUnfolding.back() == false)
            //     return;
            // if (nodeType.back() == DataType::Array)
            //     nodeIndex.back()++;
            //
            // if (value.size() + 1 > value.capacity())
            //     value.reserve(value.capacity() * 2);
            // if (ImGui::InputText(GetNodeName().c_str(), value.data(), value.capacity()))
            //     value.resize(strlen(value.data()));
        }
        void Transfer(std::vector<std::byte>& value) override
        {
            // if (nodeUnfolding.back() == false)
            //     return;
            // if (nodeType.back() == DataType::Array)
            //     nodeIndex.back()++;
            //
            // ImGui::LabelText(GetNodeName().c_str(), "binary data");
        }

        void Transfer(void* value, const std::type_index type) override
        {
            // if (nodeUnfolding.back() == false)
            //     return;
            // if (nodeType.back() == DataType::Array)
            //     nodeIndex.back()++;
            // if (type == typeid(Entity))
            // {
            //     EditorUIUtility::DrawEntityButton(*static_cast<Entity*>(value));
            //     return;
            // }
            //
            // ImGui::Text("unknown type");
        }

    private:
        // std::vector<std::string> nodePath; //节点各层级名称
        // std::vector<DataType> nodeType; //节点各层级类型
        // std::vector<bool> nodeUnfolding = {true}; //当前层级是否处于展开状态
        // std::vector<int> nodeIndex = {0}; //当前层级下当前节点的序号
        // int nodeID = 0; //节点ID，用于解决ImGuiID碰撞问题
        //
        // float dragSpeed = 0.1f;
        // //数组
        // std::vector<void*> arrayBuffer;
        // int arrayType = 0;
        //
        //
        // std::string GetNodeName(size_t index)
        // {
        //     if (nodeType.back() == DataType::Array)
        //         return std::format("({})##{}", index, nodePath.back());
        //
        //     return nodePath.back();
        // }
        // std::string GetNodeName()
        // {
        //     if (nodeType.back() == DataType::Array)
        //         return std::format("({})##{}", nodeIndex.back() - 1, nodePath.back());
        //
        //     return std::format("{}##{}", nodePath.back(), nodeID);
        // }
        //
        // void FlushArray()
        // {
        //     if (nodeUnfolding.back() == false)
        //         return;
        //     if (arrayType == 0)
        //         return;
        //
        //     if (arrayType == 1)
        //     {
        //         if (arrayBuffer.size() == 1)
        //             ImGui::DragFloat(GetNodeName().c_str(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
        //         else if (arrayBuffer.size() == 2)
        //             ImGui::DragFloat2(GetNodeName().c_str(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
        //         else if (arrayBuffer.size() == 3)
        //             ImGui::DragFloat3(GetNodeName().c_str(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
        //         else if (arrayBuffer.size() == 4)
        //             ImGui::DragFloat4(GetNodeName().c_str(), static_cast<float*>(arrayBuffer[0]), dragSpeed);
        //         else
        //         {
        //             for (size_t i = 0; i < arrayBuffer.size(); i++)
        //                 ImGui::DragFloat(GetNodeName(i).c_str(), static_cast<float*>(arrayBuffer[i]), dragSpeed);
        //         }
        //     }
        //
        //     if (arrayType == 2)
        //     {
        //         if (arrayBuffer.size() == 1)
        //             ImGui::DragInt(GetNodeName().c_str(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
        //         else if (arrayBuffer.size() == 2)
        //             ImGui::DragInt2(GetNodeName().c_str(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
        //         else if (arrayBuffer.size() == 3)
        //             ImGui::DragInt3(GetNodeName().c_str(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
        //         else if (arrayBuffer.size() == 4)
        //             ImGui::DragInt4(GetNodeName().c_str(), static_cast<int*>(arrayBuffer[0]), dragSpeed);
        //         else
        //         {
        //             for (size_t i = 0; i < arrayBuffer.size(); i++)
        //                 ImGui::DragInt(GetNodeName(i).c_str(), static_cast<int*>(arrayBuffer[i]), dragSpeed);
        //         }
        //     }
        // }
    };
}