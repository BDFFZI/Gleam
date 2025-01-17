#include "GraphicsPreset.h"

namespace Light
{
    void GraphicsPreset::Initialize()
    {
        DefaultGLDescriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(DefaultDescriptorBindings, VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR);
        DefaultGLPipelineLayout = std::make_unique<GLPipelineLayout>(*DefaultGLDescriptorSetLayout, DefaultPushConstantRanges);
        DefaultGLImageSampler = std::make_unique<GLImageSampler>();
        DefaultTexture2D = std::make_unique<Texture2D>(0.0f);
    }
    void GraphicsPreset::UnInitialize()
    {
        DefaultGLDescriptorSetLayout.reset();
        DefaultGLPipelineLayout.reset();
        DefaultGLImageSampler.reset();
        DefaultTexture2D.reset();
    }
}
