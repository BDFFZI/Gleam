#include "Rendering.h"

#include "CommandBufferPool.h"

namespace Gleam
{
    std::unique_ptr<Mesh> CreateFullScreenMesh()
    {
        std::unique_ptr<Mesh> fullScreenMesh = std::make_unique<Mesh>();
        fullScreenMesh->SetPositions(std::vector{
            float3(-1, -1, 1),
            float3(-1, 1, 1),
            float3(1, 1, 1),
            float3(1, -1, 1),
        });
        fullScreenMesh->SetUVs({
            float2(0, 1),
            float2(0, 0),
            float2(1, 0),
            float2(1, 1)
        });
        fullScreenMesh->SetIndices({
            0, 1, 2,
            0, 2, 3
        });
        return fullScreenMesh;
    }

    void Rendering::Init()
    {
        //顶点绘制
        GSCodeLayout vertexColorGSCodeLayout = GSCodeLayout("StreamingAssets/GleamRenderingRuntime/VertexColor.hlsl");
        GSInoutLayout pointGSInoutLayout = GSInoutLayout(
            RenderingConfig::PointMeshLayout, RenderingConfig::ColorFormat, RenderingConfig::DepthStencilFormat);
        GSInoutLayout lineGSInoutLayout = GSInoutLayout(
            RenderingConfig::LineMeshLayout, RenderingConfig::ColorFormat, RenderingConfig::DepthStencilFormat);
        defaultPointShader = std::make_unique<GShader>(vertexColorGSCodeLayout, pointGSInoutLayout);
        defaultLineShader = std::make_unique<GShader>(vertexColorGSCodeLayout, lineGSInoutLayout);
        defaultPointMaterial = std::make_shared<Material>(*defaultPointShader);
        defaultLineMaterial = std::make_shared<Material>(*defaultLineShader);
        //位块传输
        fullScreenMesh = CreateFullScreenMesh();
        blitShader = std::make_unique<GShader>(GSCodeLayout("StreamingAssets/GleamRenderingRuntime/Blit.hlsl"));
        blitMaterial = std::make_unique<Material>(*blitShader);
        //命令缓冲区
        CommandBufferPool::Init();
    }
    void Rendering::UnInit()
    {
        defaultPointShader.reset();
        defaultLineShader.reset();
        defaultPointMaterial.reset();
        defaultLineMaterial.reset();
        fullScreenMesh.reset();
        blitShader.reset();
        blitMaterial.reset();
        //命令缓冲区
        CommandBufferPool::UnInit();
    }

    std::weak_ptr<Material> Rendering::GetDefaultPointMaterial()
    {
        return defaultPointMaterial;
    }
    std::weak_ptr<Material> Rendering::GetDefaultLineMaterial()
    {
        return defaultLineMaterial;
    }
    const GShader& Rendering::GetDefaultPointShader()
    {
        return *defaultPointShader;
    }
    const GShader& Rendering::GetDefaultLineShader()
    {
        return *defaultLineShader;
    }
    const std::unique_ptr<Mesh>& Rendering::GetFullScreenMesh()
    {
        return fullScreenMesh;
    }
    const std::unique_ptr<Material>& Rendering::GetBlitMaterial()
    {
        return blitMaterial;
    }
}