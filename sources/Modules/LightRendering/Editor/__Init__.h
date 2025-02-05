#pragma once
#include "LightRendering/Runtime/__Init__.h"
#include "LightEngine/Editor/__Init__.h"

#include "GameWindow.h"
#include "SceneWindow.h"

namespace Light
{
    Light_AddSystems(
        GameWindow,
        SceneWindow
    )
}