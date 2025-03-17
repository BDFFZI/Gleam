#pragma once
#include "GleamEngine/Runtime/System/TimeSystem.h"

namespace Gleam
{
    inline TimeSystem& EditorTimeSystem = System::CreateGlobal<TimeSystem>("EditorTimeSystem");
}