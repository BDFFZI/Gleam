#pragma once
#include "GleamEngine/Runtime/System/TimeSystem.h"

namespace Gleam
{
    inline TimeSystem EditorTimeSystem = System::Create<TimeSystem>("EditorTimeSystem");
}