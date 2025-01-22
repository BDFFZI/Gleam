#pragma once
#include "LightEngine/Runtime/Component.h"
#include "LightPresentation/Runtime/PresentationSystem.h"
#include "LightGraphics/Runtime/GShader.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/Renderer.h"

namespace Light
{
    struct CameraInfo
    {
        float4x4 matrixVP;
        float depth;
        RenderTargetAsset* renderTarget;

        CameraInfo() = default;
        CameraInfo(Transform& transform, Camera& camera);

        bool operator<(CameraInfo& other) const
        {
            return depth > other.depth;
        }
    };

    struct RendererInfo
    {
        float4x4 matrixM;
        int renderQueue;
        Material* material;
        Mesh* mesh;

        RendererInfo() = default;
        RendererInfo(Transform& transform, Renderer& renderer);

        bool operator<(RendererInfo& other) const
        {
            return renderQueue < other.renderQueue;
        }
    };

    class RenderingSystem : public SystemGroup
    {
    public:
        RenderingSystem(): SystemGroup(PresentationSystem, LeftOrder, MiddleOrder)
        {
        }

    private:
        std::multiset<CameraInfo> cameraInfos;
        std::multiset<RendererInfo> rendererInfos;

        std::unique_ptr<GShader> defaultPointShader = nullptr;
        std::unique_ptr<GShader> defaultLineShader = nullptr;
        std::unique_ptr<GMaterial> defaultPointMaterial = nullptr;
        std::unique_ptr<GMaterial> defaultLineMaterial = nullptr;

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(RenderingSystem)
}