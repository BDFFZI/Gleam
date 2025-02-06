#pragma once
#include "LightEngine/Runtime/__Init__.h"

#include "Cursor.h"
#include "Window.h"
#include "Input.h"

namespace Light
{
    Light_AddSystems(Cursor)
    Light_AddSystems(Window)
    Light_AddSystems(Input)
}