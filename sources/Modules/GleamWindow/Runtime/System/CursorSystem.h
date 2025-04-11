#pragma once
#include "InputSystem.h"
#include "WindowSystem.h"
#include "GleamWindow/Runtime/Cursor.h"

namespace Gleam
{
    class CursorSystem : public System<WindowSystem>
    {
        void Update() override;
    };
    Gleam_MakeType(CursorSystem)
    {
        transferrer.TransferField("isLocking", Cursor::isLocking);
        transferrer.TransferField("lockingPos", Cursor::lockingPos);
    }
    Gleam_MakeRuntimeSystem(CursorSystem)
}