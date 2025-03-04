#pragma once

#include "GleamReflection/Runtime/FieldDataTransferrer.h"

namespace Gleam
{
    class EditorUISerializer : public FieldDataTransferrer
    {
    public:
        EditorUISerializer(std::string_view rootName);

        void PushNode(std::optional<std::string_view> name, DataType dataType) override;
        void PopNode() override;

        void Transfer(float& value) override;
        void Transfer(int32_t& value) override;
        void Transfer(double& value) override;
        void Transfer(int64_t& value) override;
        void Transfer(std::string& value) override;
        void Transfer(std::vector<std::byte>& value) override;
        void Transfer(void* value, std::type_index type) override;

    private:
        float dragSpeed = 0.1f;
        //倒数第一位为当前节点信息，倒数第二位为当前节点父节点信息
        std::vector<std::string> nodePaths; //当前节点层次名称
        std::vector<DataType> nodeTypes = {DataType::Class}; //当前节点层次类型
        std::vector<bool> nodeFolds = {true}; //当前节点层次展开状态
        std::vector<int> nodeIndices = {0}; //当前节点的当前成员序号
        //数组
        std::vector<void*> arrayBuffer;
        int arrayType = 0;

        std::string GetElementName(size_t index);
        std::string GetNodeName(int layerOffset = 0);
        void PreTransferNode();

        void TransferBufferedArray();
    };
}