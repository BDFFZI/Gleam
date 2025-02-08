#pragma once
#include "InputSystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class CursorSystem : public System
    {
    public:
        CursorSystem(): System(InputSystem, OrderRelation::After)
        {
        }

        bool GetLockState() const;
        float2 GetLockPosition() const;
        void SetLockState(bool state);
        void SetVisible(bool state);

    private:
        bool isLocking = false;
        float2 lockingPos = 0;
        
        void Update() override;
    };
    Light_MakeSystemInstance(CursorSystem)
}