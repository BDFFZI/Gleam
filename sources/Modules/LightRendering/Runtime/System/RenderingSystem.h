#pragma once
#include "LightEngine/Runtime/Component.h"
#include "LightPresentation/Runtime/PresentationSystem.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/Renderer.h"
#include "LightRendering/Runtime/Resource/Mesh.h"

namespace Light
{
    struct CameraInfo
    {
        Camera* camera;
        float4x4 matrixVP;

        CameraInfo() = default;
        CameraInfo(Transform& transform, Camera& camera);
        bool operator<(const CameraInfo& other) const;
    };

    struct RendererInfo
    {
        float4x4 matrixM;
        int renderQueue;
        Material* material;
        Mesh* mesh;

        RendererInfo() = default;
        RendererInfo(Transform& transform, Renderer& renderer);

        bool operator<(const RendererInfo& other) const
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

        std::unique_ptr<GSCodeLayout> vertexColorGSCodeLayout;
        std::unique_ptr<GSInoutLayout> pointGSInoutLayout;
        std::unique_ptr<GSInoutLayout> lineGSInoutLayout;
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