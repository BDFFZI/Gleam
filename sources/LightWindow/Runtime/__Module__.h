#pragma once

#include "LightEngine/Runtime/__Module__.h"

#include "Window.h"
#include "Time.h"
#include "Input.h"

namespace Light
{
    Light_AddSystems(Window)
    Light_AddSystems(Time)
    Light_AddSystems(Input)
}