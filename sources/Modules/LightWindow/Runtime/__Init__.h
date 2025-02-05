#pragma once
#include "Cursor.h"
#include "Window.h"
#include "Time.h"
#include "Input.h"
#include "LightEngine/Runtime/Engine.h"

namespace Light
{
    Light_AddSystems(Cursor)
    Light_AddSystems(Window)
    Light_AddSystems(Input)
}

#include "LightEngine/Runtime/__Init__.h"