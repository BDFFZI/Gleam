﻿#pragma once
#include "LightPresentation/Runtime/PresentationSystem.h"
#include "LightPresentation/Runtime/Graphics/Shader.h"

namespace Light
{
    class RenderingSystem : public SystemGroup
    {
    public:
        RenderingSystem(): SystemGroup(PresentationSystem, LeftOrder, MiddleOrder)
        {
        }

    private:
        std::unique_ptr<Shader> defaultPointShader = nullptr;
        std::unique_ptr<Shader> defaultLineShader = nullptr;
        std::unique_ptr<Material> defaultPointMaterial = nullptr;
        std::unique_ptr<Material> defaultLineMaterial = nullptr;
 
        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(RenderingSystem)
}