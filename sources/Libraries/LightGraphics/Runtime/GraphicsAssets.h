#pragma once
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"

namespace Light
{



    struct TextureAsset
    {

    };

    struct RenderTargetAsset
    {
        uint32_t width, height; //渲染区域
        GLImageView* glColorImageView; //颜色附件
        GLImageView* glDepthStencilImageView; //深度模板附件
        GLImageView* glColorResolveImageView; //颜色重解算附件

        /**
         * @brief 渲染后希望的布局（意图，如作为呈现源或着色器纹理）
         *
         * 由于没有使用renderPass，因此布局变换得手动设置。
         * 每次渲染后图片布局默认为“未定义”，需在使用前调整为使用所需的布局。
         * 
         * @return 
         */
        VkImageLayout glFinalLayout;
        VkImage glFinalImage;
    };
}
