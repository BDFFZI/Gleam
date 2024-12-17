#include "GameWindow.h"

#include <imgui_impl_vulkan.h>

#include "LightUI/Runtime/UI.h"

namespace Light
{
    void GameWindow::Start()
    {
    }
    void GameWindow::Stop()
    {
        if (textureID != nullptr)
            UI::DeleteTexture(textureID);
        renderTexture.reset();
    }
    void GameWindow::Update()
    {
        ImGui::Begin("GameWindow");

        const float2 windowSizeF = UI::GetWindowContentRegionSize();
        uint32_t windowSize[] = {static_cast<uint32_t>(windowSizeF.x), static_cast<uint32_t>(windowSizeF.y)};
        if (lastWindowSize[0] != windowSize[0] || lastWindowSize[1] != windowSize[1])
        {
            renderTexture = std::make_unique<RenderTexture>(windowSize[0], windowSize[1]);
            Graphics::SetDefaultRenderTarget(*renderTexture);
            if (textureID != nullptr)
                UI::DeleteTexture(textureID);
            textureID = UI::CreateTexture(*renderTexture);
            lastWindowSize[0] = windowSize[0];
            lastWindowSize[1] = windowSize[1];
        }

        ImGui::Image(textureID, windowSizeF);

        ImGui::End();
    }
}
