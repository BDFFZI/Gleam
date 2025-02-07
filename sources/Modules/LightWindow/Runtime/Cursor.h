#pragma once
#include "Input.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class Cursor : public System
    {
    public:
        Cursor(): System(Input, OrderRelation::After)
        {
        }

        bool GetLockState() const;
        float2 GetLockPosition() const;
        void SetLockState(bool state);
        void SetVisible(bool state);

    private:
        bool isLocking = false;
        float2 lockingPos;

        void Update() override;
    };
    Light_MakeGlobalSystem(Cursor)
}