#include <typeindex>

#include "LightMath/Runtime/Vector.hpp"
class Transferrer
{
public:
    virtual ~Transferrer() = default;
    template <class TField>
    void TransferField(const char* name, TField& value)
    {
        TransferField(name, value, typeid(TField));
    }
    void TransferField(const char* name, int& value)
    
    virtual void TransferValue(void& value, std::type_index type) = 0;
    virtual void TransferBool(bool& value) = 0;
    virtual void TransferChar(bool& value) = 0;
    virtual void TransferInt(bool& value) = 0;
    virtual void TransferFloat(bool& value) = 0;

    virtual void PushPath() = 0;
    virtual void PopPath() = 0;
};
class PrintTransmitter : public Transferrer
{
public:
    void TransferField(const char* name, void& value, std::type_index type) override;
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
}
