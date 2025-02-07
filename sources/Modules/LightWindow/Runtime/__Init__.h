#pragma once
#include "LightEngine/Runtime/__Init__.h"

#include "Cursor.h"
#include "Window.h"
#include "Input.h"

namespace Light
{
    Light_AddEditorSystems(Cursor)
    Light_AddEditorSystems(Window)
    Light_AddEditorSystems(Input)
}