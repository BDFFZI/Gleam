#pragma once
#include "LightECS/Runtime/System.h"

namespace Light
{
    class DestroySystem : public System
    {
    public:
        DestroySystem(): System(std::nullopt, RightOrder, RightOrder)
        {
        }

    private:
        void Update() override
        {
            Destroy->RemoveAllEntities();
        }
    };
    Light_MakeSystem(DestroySystem)
}