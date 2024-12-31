#pragma once
#include "RenderSkyboxSystem.h"
#include "LightPresentation/Runtime/Graphics/Mesh.h"
#include "LightPresentation/Runtime/Graphics/Shader.h"

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
        std::unique_ptr<Material> pointMaterial = nullptr;
        std::unique_ptr<Material> lineMaterial = nullptr;
    };
    Light_MakeSystem(RenderOpaqueSystem)
}