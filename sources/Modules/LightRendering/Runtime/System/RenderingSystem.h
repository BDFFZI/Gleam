#pragma once
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightGraphics/Runtime/Resource/GRenderTarget/GRenderTexture.h"
#include "LightPresentation/Runtime/PresentationSystem.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/Renderer.h"
#include "LightRendering/Runtime/Resource/Mesh.h"

namespace Light
{
    struct CameraInfo
    {
        Camera* camera;
        CameraTransform* cameraTransform;

        CameraInfo() = default;
        CameraInfo(Camera& camera, CameraTransform& cameraTransform);
        bool operator<(const CameraInfo& other) const;
    };

    struct RendererInfo
    {
        float4x4 localToWorld;
        int renderQueue;
        Material* material;
        Mesh* mesh;

        RendererInfo() = default;
        RendererInfo(const float4x4& localToWorld, Renderer& renderer);

        bool operator<(const RendererInfo& other) const;
    };

    class RenderingSystem : public System
    {
    public:
        RenderingSystem(): System(PresentationSystem, LeftOrder, MiddleOrder)
        {
        }

        const std::unique_ptr<Material>& GetDefaultPointMaterial() const;
        const std::unique_ptr<Material>& GetDefaultLineMaterial() const;
        const std::unique_ptr<Mesh>& GetFullScreenMesh() const;
        const std::unique_ptr<Material>& GetBlitMaterial() const;
        GRenderTarget* GetDefaultRenderTarget() const;

        void SetDefaultRenderTarget(GRenderTarget* renderTarget);

    private:
        std::multiset<CameraInfo> cameraInfos;
        std::multiset<RendererInfo> rendererInfos;

        GRenderTarget* defaultRenderTarget = nullptr;
        //点线绘制
        std::unique_ptr<GShader> defaultPointShader = nullptr;
        std::unique_ptr<GShader> defaultLineShader = nullptr;
        std::unique_ptr<Material> defaultPointMaterial = nullptr;
        std::unique_ptr<Material> defaultLineMaterial = nullptr;
        //位块传输
        std::unique_ptr<Mesh> fullScreenMesh;
        std::unique_ptr<GShader> blitShader = nullptr;
        std::unique_ptr<Material> blitMaterial = nullptr;

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(RenderingSystem)
}