#pragma once
#include "GleamEngine/Runtime/Entity/Transform.h"
#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamRendering/Runtime/Asset/Material.h"
#include "GleamRendering/Runtime/Entity/Camera.h"
#include "GleamRendering/Runtime/Entity/Renderer.h"
#include "GleamRendering/Runtime/Asset/Mesh.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    struct CameraInfo
    {
        Camera* camera;
        WorldToClip* worldToClip;

        CameraInfo() = default;
        CameraInfo(Camera& camera, WorldToClip& worldToClip);
        bool operator<(const CameraInfo& other) const;
    };

    struct RendererInfo
    {
        float4x4 localToWorld;
        int renderQueue;
        Material* material;
        Mesh* mesh;
        uint32_t instanceCount = 1;

        RendererInfo() = default;
        RendererInfo(const float4x4& localToWorld, Renderer& renderer);
        RendererInfo(const float4x4& localToWorld, RenderQueue renderQueue, Material& material, Mesh& mesh, uint32_t instanceID = 1);

        bool operator<(const RendererInfo& other) const;
    };

    class RenderingSystem : public System<PresentationSystem>
    {
    public:
        GRenderTarget& GetDefaultRenderTarget() const;
        void SetDefaultRenderTarget(GRenderTarget& renderTarget);

        void AddRendererInfo(const RendererInfo& rendererInfo);
        void AddRendererInfos(const std::vector<RendererInfo>& rendererInfos);

    private:
        GRenderTarget* defaultRenderTarget = {};
        std::multiset<CameraInfo> cameraInfos;
        std::multiset<RendererInfo> rendererInfos;

        void Start() override;
        void Update() override;
    };
    
#ifdef GleamEngineEditor
    Gleam_MakeEditorSystem(RenderingSystem)
#else
    Gleam_MakeRuntimeSystem(RenderingSystem)
#endif
}