#pragma once

#include "GleamReflection/Runtime/FieldDataTransferrer.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    class EditorUISerializer : public FieldDataTransferrer
    {
    public:
        EditorUISerializer(std::string_view rootName);

        void PushNode(std::optional<std::string_view> name, DataType dataType) override;
        void PopNode() override;

        void TransferNumber(void* address, int size, ImGuiDataType dataType);
        void Transfer(int32_t& value) override
        {
            TransferNumber(&value, sizeof(value), ImGuiDataType_S32);
        }
        void Transfer(int64_t& value) override
        {
            TransferNumber(&value, sizeof(value), ImGuiDataType_S64);
        }
        void Transfer(uint32_t& value) override
        {
            TransferNumber(&value, sizeof(value), ImGuiDataType_U32);
        }
        void Transfer(uint64_t& value) override
        {
            TransferNumber(&value, sizeof(value), ImGuiDataType_U64);
        }
        void Transfer(float& value) override
        {
            TransferNumber(&value, sizeof(value), ImGuiDataType_Float);
        }
        void Transfer(double& value) override
        {
            TransferNumber(&value, sizeof(value), ImGuiDataType_Double);
        }
        void Transfer(char& value) override
        {
            TransferNumber(&value, sizeof(value), ImGuiDataType_S8);
        }
        
        void Transfer(bool& value) override;
        void Transfer(std::string& value) override;
        void Transfer(std::vector<std::byte>& value) override;
        void FallbackTransfer(void* value, std::type_index typeIndex) override;
        void FallbackTransferPtr(std::weak_ptr<void>& value, std::type_index objectTypeIndex) override;

    private:
        float dragSpeed = 0.1f;
        //倒数第一位为当前节点信息，倒数第二位为当前节点父节点信息
        std::vector<std::string> nodePaths; //当前节点层次名称
        std::vector<DataType> nodeTypes = {DataType::Class}; //当前节点层次类型
        std::vector<bool> nodeFolds = {true}; //当前节点层次展开状态
        std::vector<int> nodeIndices = {0}; //当前节点的当前成员序号
        //数组
        std::vector<void*> bufferedVectorItem;
        ImGuiDataType bufferedVectorItemType = -1;

        std::string GetElementName(size_t index);
        std::string GetNodeName(int layerOffset = 0);
        void PreTransferNode();

        void PushBufferedVectorItem(void* itemAddress, int itemSize, ImGuiDataType itemType);
        void TransferBufferedVector();
    };
}