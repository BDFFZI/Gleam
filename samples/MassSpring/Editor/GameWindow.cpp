#include "GameWindow.h"

#include <imgui.h>

namespace Light
{
    void GameWindow::Start()
    {
        renderTexture = std::make_unique<RenderTexture>()
    }
    void GameWindow::Update()
    {
        ImGui::Begin("GameWindow");

        ImGui::GetWindowSize()

        ImGui::End();
    }
}
