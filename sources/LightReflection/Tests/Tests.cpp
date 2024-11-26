#include <iostream>
#include <ostream>
#include <string>
#include <typeindex>

#include "LightMath/Runtime/Vector.hpp"
class Transferrer
{
public:
    virtual ~Transferrer() = default;

    template <class TField>
    void TransferField(const char* name, TField& value)
    {
        PushPath(name);
        TransferValue(value);
        PopPath();
    }
    template <class TField>
    void TransferValue(TField& value)
    {
        TransferValue(&value, typeid(TField));
    }

    virtual void TransferValue(void* value, std::type_index type) = 0;
    virtual void PushPath(const char* path)
    {
    }
    virtual void PopPath()
    {
    }
};
class PrintTransferrer : public Transferrer
{
public:
    void TransferValue(void* value, const std::type_index type) override
    {
        if(type == typeid(float))
        {
            TransferFloat(*static_cast<float*>(value));
        }

    }
    void TransferFloat(float& value)
    {
        std::cout << value << " ";
    }
};

struct Data
{
    bool boolValue = true;
    char charValue = 'A';
    int intValue = 1;
    float floatValue = 0.5f;
    float3 float3Value = {1, 2, 3};
    std::vector<bool> boolVectorValue = {false, true};
};

void main()
{
    Data data = {};

    PrintTransferrer transferrer;
    transferrer.TransferField("boolValue", data.boolValue);
    transferrer.TransferField("charValue", data.charValue);
    transferrer.TransferField("intValue", data.intValue);
    transferrer.TransferField("floatValue", data.floatValue);
}
