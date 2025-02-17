#pragma once
#include "GleamEngine/Runtime/Component/Transform.h"
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
        RendererInfo(const float4x4& localToWorld, RenderQueue renderQueue, Material& material, Mesh& mesh);

        bool operator<(const RendererInfo& other) const;
    };

    class RenderingSystem : public System
    {
    public:
        RenderingSystem(): System(PresentationSystem)
        {
        }

        GRenderTarget* GetDefaultRenderTarget() const;
        void SetDefaultRenderTarget(GRenderTarget* renderTarget);

        void AddRendererInfo(const RendererInfo& rendererInfo);

    private:
        GRenderTarget* defaultRenderTarget = nullptr;
        std::multiset<CameraInfo> cameraInfos;
        std::multiset<RendererInfo> rendererInfos;

        void Start() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(RenderingSystem)
}