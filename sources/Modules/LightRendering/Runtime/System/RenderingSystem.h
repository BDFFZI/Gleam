#pragma once
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightGraphics/Runtime/Resource/GRenderTarget/GRenderTexture.h"
#include "LightPresentation/Runtime/PresentationSystem.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/Renderer.h"
#include "LightRendering/Runtime/Asset/Mesh.h"

namespace Light
{
    struct CameraInfo
    {
        Camera* camera;
        WorldToClip* cameraTransform;

        CameraInfo() = default;
        CameraInfo(Camera& camera, WorldToClip& cameraTransform);
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

        GRenderTarget* GetDefaultRenderTarget() const;
        void SetDefaultRenderTarget(GRenderTarget* renderTarget);

    private:
        GRenderTarget* defaultRenderTarget = nullptr;
        std::multiset<CameraInfo> cameraInfos;
        std::multiset<RendererInfo> rendererInfos;

        void Start() override;
        void Update() override;
    };
    Light_MakeSystemInstance(RenderingSystem)
}