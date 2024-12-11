#pragma once
#include <memory>
#include "Texture.h"
#include "LightGL/Runtime/Resource/GLImageView.h"

namespace Light
{
    class RenderTargetBase
    {
    public:
        virtual ~RenderTargetBase() = default;
        virtual uint32_t GetWidth() const = 0; //渲染区域-宽
        virtual uint32_t GetHeight() const = 0; //渲染区域-高
        virtual const GLImageView& GetGLColorImageView() const = 0; //颜色附件
        virtual const GLImageView* GetGLDepthStencilImageView() const = 0; //深度模板附件
        virtual const GLImageView* GetGLColorResolveImageView() const = 0; //颜色重解算附件
        /**
         * @brief 渲染后希望的布局（意图，如作为呈现源或着色器纹理）
         *
         * 由于没有使用renderPass，因此布局变换得手动设置。
         * 每次渲染后图片布局默认为“未定义”，需在使用前调整为使用所需的布局。
         * 
         * @return 
         */
        virtual VkImageLayout GetTargetLayout() const = 0;

        float GetAspectRatio() const { return static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()); }
    };

    struct RenderTarget : RenderTargetBase
    {
        uint32_t width, height;
        const GLImageView* glColorImageView;
        const GLImageView* glDepthStencilImageView;
        const GLImageView* glColorResolveImageView;
        VkImageLayout imageLayout;

        uint32_t GetWidth() const override { return width; }
        uint32_t GetHeight() const override { return height; }
        const GLImageView& GetGLColorImageView() const override { return *glColorImageView; }
        const GLImageView* GetGLDepthStencilImageView() const override { return glDepthStencilImageView; }
        const GLImageView* GetGLColorResolveImageView() const override { return glColorResolveImageView; }
        VkImageLayout GetTargetLayout() const override { return imageLayout; }
    };

    class RenderTexture : public RenderTargetBase, public TextureBase
    {
    public:
        RenderTexture(
            uint32_t width, uint32_t height,
            VkFormat colorFormat,
            VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED,
            VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
        RenderTexture(const RenderTexture&) = delete;

        //支持作为渲染目标
        uint32_t GetWidth() const override { return width; }
        uint32_t GetHeight() const override { return height; }
        const GLImageView& GetGLColorImageView() const override { return *glColorImageView; }
        const GLImageView* GetGLDepthStencilImageView() const override { return glDepthStencilImageView.get(); }
        const GLImageView* GetGLColorResolveImageView() const override { return glColorResolveImageView.get(); }

        //支持作为纹理
        const GLImageView& GetGLImageView() const override { return *glColorImageView; }
        const GLImageSampler& GetGLImageSampler() const override { return *glImageSampler; }

        // VkSampleCountFlagBits GetSampleCount() const override { return sampleCount; }
        // const VkImage& GetVKColorImage() const override;
        // const VkImage* GetVkDepthStencilImage() const override;
        // const VkImage* GetVkColorResolveImage() const override;

    private:
        uint32_t width, height;
        VkSampleCountFlagBits sampleCount;
        std::unique_ptr<GLImageSampler> glImageSampler;
        std::unique_ptr<GLImage> glColorImage;
        std::unique_ptr<GLImage> glDepthStencilImage;
        std::unique_ptr<GLImage> glColorResolveImage;
        std::unique_ptr<GLImageView> glColorImageView;
        std::unique_ptr<GLImageView> glDepthStencilImageView;
        std::unique_ptr<GLImageView> glColorResolveImageView;
    };
}
