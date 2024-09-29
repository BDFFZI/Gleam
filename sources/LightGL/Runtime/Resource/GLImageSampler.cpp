#include "GLImageSampler.h"

#include <stdexcept>

#include "../GL.h"

GLImageSampler::GLImageSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    //采样模式，此处都采样线性插值，即双线性
    samplerInfo.magFilter = VK_FILTER_LINEAR; //图片像素大小大于采样像素大小时（欠采样，相机过近，lod为0）的采样方式
    samplerInfo.minFilter = VK_FILTER_LINEAR; //图片像素大小低于采样像素大小时（过采样，相机过远）的采样方式
    //uv包裹模式
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    //各向异性
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = GL::glDevice->deviceProperties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; //当使用夹紧uv包裹模式时，对于超出编辑的uv应采样到的颜色
    samplerInfo.unnormalizedCoordinates = VK_FALSE; //是否使用非归一化uv坐标，即直接用像素长宽采样。通常都不使用这种模式
    //像素比较功能，启用后会先将像素与特定值比较，然后在后续采样中使用该结果，常用于阴影贴图实现，但一般不使用
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    //针对mipmap的采样模式
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; //一般用线性，此时再加上上述的双线性即实现了俗称的三线性采样
    samplerInfo.mipLodBias = 0.0f; //lod偏移（lod是一个浮点数，但最终会向下取整以得出对应的mipLevel）
    samplerInfo.minLod = 0.0f; //最小lod
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE; //最大lod

    if (vkCreateSampler(GL::glDevice->device, &samplerInfo, nullptr, &imageSampler) != VK_SUCCESS)
        throw std::runtime_error("创建图片采样器失败！");
}
GLImageSampler::~GLImageSampler()
{
    vkDestroySampler(GL::glDevice->device, imageSampler, nullptr);
}
