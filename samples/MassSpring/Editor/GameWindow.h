#pragma once
#include "LightUI/Runtime/UI.h"
#include "LightECS/Runtime/_Template.hpp"
#include "../Systems/UISystem.h"

namespace Light
{
    class GameWindow : public SystemT<UISystemGroup>
    {
    public:
        static void Start();
        static void Stop();
        static void Update();

    private:
        inline static uint32_t lastWindowSize[2] = {0, 0};
        inline static std::unique_ptr<RenderTexture> renderTexture = nullptr;
        inline static std::unique_ptr<GLImageSampler> imageSampler = nullptr;
        inline static VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    };
}
