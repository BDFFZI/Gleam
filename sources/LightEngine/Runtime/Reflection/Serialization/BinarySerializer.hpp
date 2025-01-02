#pragma once
#include "Serializer.hpp"

namespace Light
{
    class BinarySerializer : public Serializer
    {
    public:
        void PushNode(const char* name, NodeType nodeType) override
        {
        }
        void PopNode() override
        {
        }
    };
}