#include "RenderingSystem.h"

namespace Light
{
    void RenderingSystem::Start()
    {
        pointShader = std::make_unique<Shader>("Assets/VertexColor.hlsl", GraphicsPreset::DefaultStateLayout, pointMeshLayout);
        lineShader = std::make_unique<Shader>("Assets/VertexColor.hlsl", GraphicsPreset::DefaultStateLayout, lineMeshLayout);
        pointMaterial = std::make_unique<Material>(*pointShader);
        lineMaterial = std::make_unique<Material>(*lineShader);
        
        SystemGroup::Start();
    }
    void RenderingSystem::Stop()
    lit{
        SystemGroup::Stop();
    }
    void RenderingSystem::Update()
    {
        SystemGroup::Update();
    }
}