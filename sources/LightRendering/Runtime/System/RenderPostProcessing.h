#pragma once
#include "RenderTransparentSystem.h"

namespace Light
{
    class RenderPostProcessing : public System
    {
    public:
        RenderPostProcessing(): System(RenderTransparentSystem, OrderRelation::After)
        {
        }
    };
    Light_MakeSystem(RenderPostProcessing)
}