#pragma once
#include "GraphicsPreset.hpp"
#include "LightGL/Runtime/GL.h"
#include "LightUtility/Runtime/ObjectPool.hpp"
#include "Resource/GRenderTarget/GRenderTarget.hpp"
#include "Resource/GTexture/GTexture2D.h"

namespace Light
{
    class Graphics
    {
    public:
        static void InitializeGLDemand(std::vector<const char*>& extensions);

        static Graphics Initialize(GL&, const GraphicsPreset& defaultPreset);
        static void UnInitialize();

        static GraphicsPreset& GetDefaultGraphicsPreset() { return defaultGraphicsPreset; }
        static GLImageSampler& GetDefaultGLImageSampler() { return *defaultGLImageSampler; }
        static GTexture2D& getDefaultTexture2D() { return *defaultTexture2D; }
        static GRenderTarget& GetDefaultRenderTarget() { return *defaultRenderTarget; }

        static void SetDefaultRenderTarget(GRenderTarget& renderTarget) { defaultRenderTarget = &renderTarget; }

    private:
        //预创建的默认资源
        inline static GraphicsPreset defaultGraphicsPreset = {};
        inline static std::unique_ptr<GLImageSampler> defaultGLImageSampler = {};
        inline static std::unique_ptr<GTexture2D> defaultTexture2D = {};
        inline static GRenderTarget* defaultRenderTarget = nullptr;

        Graphics() = default;
    };
}