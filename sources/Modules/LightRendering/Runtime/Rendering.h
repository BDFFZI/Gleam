#pragma once
#include "Asset/Material.h"
#include "Asset/Mesh.h"
#include "LightGraphics/Runtime/Resource/GRenderTarget/GRenderTarget.h"

namespace Light
{
    class Rendering
    {
    public:
        static void Init();
        static void UnInit();

        static const std::unique_ptr<Material>& GetDefaultPointMaterial();
        static const std::unique_ptr<Material>& GetDefaultLineMaterial();
        static const std::unique_ptr<Mesh>& GetFullScreenMesh();
        static const std::unique_ptr<Material>& GetBlitMaterial();

    private:
        //点线绘制
        inline static std::unique_ptr<GShader> defaultPointShader = nullptr;
        inline static std::unique_ptr<GShader> defaultLineShader = nullptr;
        inline static std::unique_ptr<Material> defaultPointMaterial = nullptr;
        inline static std::unique_ptr<Material> defaultLineMaterial = nullptr;
        //位块传输
        inline static std::unique_ptr<Mesh> fullScreenMesh;
        inline static std::unique_ptr<GShader> blitShader = nullptr;
        inline static std::unique_ptr<Material> blitMaterial = nullptr;
    };
}