#include "GameWindow.h"

#include <imgui_impl_vulkan.h>

#include "LightUI/Runtime/UI.h"

namespace Light
{
    void GameWindow::Start()
    {
        imageSampler = std::make_unique<GLImageSampler>();
    }
    void GameWindow::Stop()
    {
        ImGui_ImplVulkan_RemoveTexture(descriptorSet);
        renderTexture.reset();
        imageSampler.reset();
    }
    void GameWindow::Update()
    {
        ImGui::Begin("GameWindow");

        float2 windowSizeF = ImGui::GetWindowSize();
        uint32_t windowSize[] = {static_cast<uint32_t>(windowSizeF.x), static_cast<uint32_t>(windowSizeF.y)};
        if (lastWindowSize[0] != windowSize[0] || lastWindowSize[1] != windowSize[1])
        {
            renderTexture = std::make_unique<RenderTexture>(windowSize[0], windowSize[1]);
            if (descriptorSet != VK_NULL_HANDLE) ImGui_ImplVulkan_RemoveTexture(descriptorSet);
            descriptorSet = ImGui_ImplVulkan_AddTexture(
                imageSampler->imageSampler,
                renderTexture->GetGLColorImageView().imageView,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );
            lastWindowSize[0] = windowSize[0];
            lastWindowSize[1] = windowSize[1];
        }

        ImGui::End();
    }
}
