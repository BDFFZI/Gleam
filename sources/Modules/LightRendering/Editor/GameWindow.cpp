#include "GameWindow.h"

#include <imgui_impl_vulkan.h>

#include "LightECS/Runtime/View.h"
#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/System/TimeSystem.h"
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
            //重建渲染目标和纹理
            if (isDirty && windowSize.x > 0 && windowSize.y > 0)
            {
                renderTexture = std::make_unique<GRenderTexture>(static_cast<int2>(windowSize));
                RenderingSystem->SetDefaultRenderTarget(renderTexture.get());
                if (renderTextureID != nullptr)
                    UI::DeleteTexture(renderTextureID);
                renderTextureID = UI::CreateTexture(*renderTexture);
            }
            //更新输入系统的焦点范围为GameWindow
            Input->SetFocusArea(Rect{windowPosition, windowSize});
        };
        World::AddSystem(&systemEvent);
    }
    void GameWindow::Stop()
    {
        World::RemoveSystem(&systemEvent);
        renderTexture.reset();
        UI::DeleteTexture(renderTextureID);
    }
    void GameWindow::Update()
    {
        ImGui::Begin("GameWindow");
        //重建纹理
        if (any(windowSize != UI::GetWindowContentRegionSize()))
            isDirty = true;
        //获取窗口信息
        windowSize = UI::GetWindowContentRegionSize();
        windowPosition = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
        //显示游戏画面
        float2 cursor = ImGui::GetCursorPos();
        int cameraCount = 0;
        View<Camera>::Each([&cameraCount](Camera& camera)
        {
            if (camera.renderTarget == std::nullopt)
                cameraCount++;
        });
        if (cameraCount != 0)
        {
            if (renderTextureID != nullptr)
                ImGui::Image(renderTextureID, windowSize);
        }
        else
        {
            ImGui::SetCursorPosY(windowSize.y / 2);
            ImGui::Text("No Camera Direct Render To Screen");
        }
        //帧率信息
        static double deltaTime = 0;
        deltaTime = std::lerp(deltaTime, TimeSystem->GetDeltaTime(), 0.05f);
        ImGui::SetCursorPos(cursor);
        ImGui::TextColored(
            float4::Magenta(),
            "Frame Rate %5.1f ms/frame (%5.1f FPS)",
            deltaTime * 1000.0,
            1.0 / deltaTime
        );
        ImGui::End();
    }
}