#include "GameWindow.h"

#include <imgui_impl_vulkan.h>

#include "LightUI/Runtime/UI.h"

namespace Light
{
    void GameWindow::Update()
    {
        ImGui::Begin("GameWindow");
        //获取窗口信息
        windowSize[0] = windowSize[1];
        windowSize[1] = UI::GetWindowContentRegionSize();
        isWindowFocused[0] = isWindowFocused[1];
        isWindowFocused[1] = ImGui::IsWindowFocused();
        windowOrigin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
        //显示游戏画面
        if (textureID != nullptr)
            ImGui::Image(textureID, windowSize[1]);

        ImGui::End();
    }

    void GameWindowAssetsSystem::Update()
    {
        //更新渲染目标
        if (any(GameWindow.windowSize[0] != GameWindow.windowSize[1]) != all(GameWindow.windowSize[1]))
        {
            GameWindow.renderTexture = std::make_unique<RenderTexture>(GameWindow.windowSize[1].x, GameWindow.windowSize[1].y);
            Graphics::SetDefaultRenderTarget(*GameWindow.renderTexture);
            if (GameWindow.textureID != nullptr)
                UI::DeleteTexture(GameWindow.textureID);
            GameWindow.textureID = UI::CreateTexture(*GameWindow.renderTexture);
        }

        //更新输入系统的鼠标原点
        Input::SetMouseOrigin(GameWindow.windowOrigin);

        //更新输入处理器
        // if (GameWindow.isWindowFocused[0] != GameWindow.isWindowFocused[1])
        // {
        //     if (GameWindow.isWindowFocused[1] == false)
        //         Input::PushInputHandler(GameWindow.inputHandler);
        //     else
        //         Input::PopInputHandler(GameWindow.inputHandler);
        //     std::cout << GameWindow.isWindowFocused[1] << std::endl;
        // }
    }
    void GameWindowAssetsSystem::Stop()
    {
        if (GameWindow.textureID != nullptr)
            UI::DeleteTexture(GameWindow.textureID);
        GameWindow.renderTexture.reset();
    }
}