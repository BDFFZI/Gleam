#pragma once
#include <cassert>
#include "LightGL/Runtime/GL.h"
#include "Resource/GRenderTarget/GRenderTarget.h"
#include "Resource/GTexture/GTexture2D.h"
#include "Resource/GShader/GSLayout.h"

namespace Light
{
    struct GraphicsConfig
    {
        VkFormat presentColorFormat = VK_FORMAT_B8G8R8A8_SRGB;
        VkFormat presentDepthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
        VkSampleCountFlagBits presentSampleCount = VK_SAMPLE_COUNT_1_BIT;
        GSAssetLayout defaultGSAssetLayout = {};
        GSInoutLayout defaultGSInoutLayout = {
            {GLVertexInput(0, {}), GLInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false)},
            presentColorFormat,
            presentDepthStencilFormat
        };
        GSStateLayout defaultGSStateLayout = []
        {
            GLStateLayout layout;
            //实现动态窗口大小
            layout.dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            layout.dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
            layout.SetViewportCount(1, 1);
            return layout;
        }();
    };

    class Graphics
    {
    public:
        static void InitializeGLDemand(std::vector<const char*>& extensions);

        static Graphics Initialize(GL&, std::unique_ptr<GraphicsConfig>&& defaultPreset);
        static void UnInitialize();

        static GraphicsConfig& GetGraphicsConfig()
        {
            assert(graphicsConfig != nullptr && "Graphics模块尚未初始化！");
            return *graphicsConfig;
        }
        static GLImageSampler& GetDefaultGLImageSampler() { return *defaultGLImageSampler; }
        static GTexture2D& GetDefaultTexture2D() { return *defaultTexture2D; }
        static GRenderTarget& GetDefaultRenderTarget() { return *defaultRenderTarget; }

        static void SetDefaultRenderTarget(GRenderTarget& renderTarget) { defaultRenderTarget = &renderTarget; }

    private:
        //预创建的默认资源
        inline static std::unique_ptr<GraphicsConfig> graphicsConfig = {};
        inline static std::unique_ptr<GLImageSampler> defaultGLImageSampler = {};
        inline static std::unique_ptr<GTexture2D> defaultTexture2D = {};
        inline static GRenderTarget* defaultRenderTarget = nullptr;

        Graphics() = default;
    };
}