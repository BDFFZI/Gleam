﻿#pragma once

#include "LightPresentation/Runtime/PresentationSystem.h"
#include "LightEngine/Runtime/ECS/System.h"

namespace Light
{
    class UISystem : public SystemGroup
    {
    public:
        UISystem(): SystemGroup(PresentationSystem, MiddleOrder, RightOrder)
        {
        }

    private:
        std::unique_ptr<GLDescriptorSetLayout> descriptorSetLayout = {};
        std::unique_ptr<GLDescriptorPool> descriptorPool = {};

        void Start() override;
        void Stop() override;
        void Update() override;
    };
}