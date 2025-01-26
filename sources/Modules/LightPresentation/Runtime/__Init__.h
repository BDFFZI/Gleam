#pragma once
#include "PresentationSystem.h"

namespace Light
{
    //通过修改该变量可以调整Presentation初始化Graphics库的方式，UI不需要，但Render需要
    inline std::function CreateGraphicsConfig = [] { return std::make_unique<GraphicsConfig>(); };

    Light_AddSystems(PresentationSystem)
}

#include "LightWindow/Runtime/__Init__.h"