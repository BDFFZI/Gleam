#pragma once
#include "InputSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class CursorSystem : public System
    {
    public:
        CursorSystem(): System(GlobalInputSystem, OrderRelation::After)
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
    Gleam_MakeGlobalSystem(CursorSystem)
}