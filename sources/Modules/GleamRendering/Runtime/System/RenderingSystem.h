#pragma once
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamGraphics/Runtime/Resource/GRenderTarget/GRenderTexture.h"
#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamRendering/Runtime/Component/Camera.h"
#include "GleamRendering/Runtime/Component/Renderer.h"
#include "GleamRendering/Runtime/Asset/Mesh.h"

namespace Gleam
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
    Gleam_MakeSystemInstance(RenderingSystem)
}