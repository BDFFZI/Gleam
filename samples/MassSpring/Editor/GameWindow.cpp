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
    }
    void GameWindow::Update()
    {
        ImGui::Begin("GameWindow");

        const float2 windowSizeF = UI::GetWindowContentRegionSize();
        lastWindowSize[0] = windowSize[0];
        lastWindowSize[1] = windowSize[1];
        windowSize[0] = {static_cast<uint32_t>(windowSizeF.x)};
        windowSize[1] = {static_cast<uint32_t>(windowSizeF.y)};

        const float2 windowOrigin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
        Input::SetMouseOrigin(windowOrigin);

        if (textureID != nullptr)
            ImGui::Image(textureID, windowSizeF);

        ImGui::End();
    }

    void GameWindowLogic::Update()
    {
        if (GameWindow.lastWindowSize[0] != GameWindow.windowSize[0] || GameWindow.lastWindowSize[1] != GameWindow.windowSize[1])
        {
            renderTexture = std::make_unique<RenderTexture>(GameWindow.windowSize[0], GameWindow.windowSize[1]);
            Graphics::SetDefaultRenderTarget(*renderTexture);
            if (GameWindow.textureID != nullptr)
                UI::DeleteTexture(GameWindow.textureID);
            GameWindow.textureID = UI::CreateTexture(*renderTexture);
        }
    }
    void GameWindowLogic::Stop()
    {
        renderTexture.reset();
    }
}
