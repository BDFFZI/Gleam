#pragma once
#include "Window.h"
#include "Time.h"
#include "Input.h"

namespace Light
{
    Light_AddSystems(Window)
    Light_AddSystems(Time)
    Light_AddSystems(Input)
}

#include "LightEngine/Runtime/__Init__.h"