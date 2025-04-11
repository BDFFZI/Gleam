#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    class Cursor
    {
    public:
        static bool GetLockState();
        static float2 GetLockPosition();
        static void SetLockState(bool state);
        static void SetVisible(bool state);

        static void Update();
    private:
        Gleam_MakeType_Friend
        
        inline static bool isLocking = false;
        inline static float2 lockingPos = 0;
    };
}