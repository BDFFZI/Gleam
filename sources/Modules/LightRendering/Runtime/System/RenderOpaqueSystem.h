﻿#pragma once
#include "RenderSkyboxSystem.h"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightGraphics/Runtime/Shader.h"

namespace Light
{
    class RenderOpaqueSystem : public System
    {
    public:
        RenderOpaqueSystem(): System(RenderSkyboxSystem, OrderRelation::Before)
        {
        }
    private:



        float orthoSize = 100.0f;

        std::unique_ptr<Shader> pointShader = nullptr;
        std::unique_ptr<Shader> lineShader = nullptr;
        std::unique_ptr<GMaterial> pointMaterial = nullptr;
        std::unique_ptr<GMaterial> lineMaterial = nullptr;
    };
    Light_MakeSystem(RenderOpaqueSystem)
}