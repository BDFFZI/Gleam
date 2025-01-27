#include "GameWindow.h"

#include <imgui_impl_vulkan.h>

#include "LightECS/Runtime/World.h"
#include "LightGraphics/Runtime/SwapChain.h"
#include "LightRendering/Runtime/System/RenderingSystem.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Input.h"

namespace Light
{
    void GameWindow::Start()
    {
        systemEvent.onUpdate = [this]
        {
            //更新输入系统的焦点范围为GameWindow
            Input->SetFocusArea(Rect{windowPosition, windowSize});
        };
        World::AddSystem(&systemEvent);
    }
    void GameWindow::Stop()
    {
        World::RemoveSystem(&systemEvent);

        UI::DeleteTexture(textureID);
    }
    void GameWindow::Update()
    {
        //重建纹理
        if (lastRenderTexture != RenderingSystem->GetDefaultRenderTarget().get())
        {
            lastRenderTexture = RenderingSystem->GetDefaultRenderTarget().get();
            if (textureID != nullptr)
                UI::DeleteTexture(textureID);
            textureID = UI::CreateTexture(*lastRenderTexture);
        }

        ImGui::Begin("GameWindow");
        //获取窗口信息
        windowSize = UI::GetWindowContentRegionSize();
        windowPosition = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
        //显示游戏画面
        ImGui::Image(textureID, windowSize);
        ImGui::End();
    }
}