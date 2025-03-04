#include "GameWindow.h"

#include <imgui_impl_vulkan.h>

#include "SceneWindow.h"
#include "GleamECS/Runtime/View.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamGraphics/Runtime/SwapChain.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    void GameWindow::Start()
    {
        windowSize = 0; //以便重启时能触发纹理重建
        preProcessSystem.OnUpdate() = [this]
        {
            //重建渲染目标和纹理
            if (isDirty && windowSize.x > 0 && windowSize.y > 0)
            {
                isDirty = false;
                SwapChain::WaitPresent();
                if (renderTextureID != nullptr)
                    UI::DeleteTexture(renderTextureID);
                renderTexture = std::make_unique<GRenderTexture>(static_cast<int2>(windowSize));
                renderTextureID = UI::CreateTexture(*renderTexture);
                GlobalRenderingSystem.SetDefaultRenderTarget(*renderTexture);
            }
            //更新输入系统的焦点范围为GameWindow
            GlobalInputSystem.SetFocusArea(Rectangle::CreateFromOrigin(windowPosition, windowSize));
        };
        World::AddSystem(preProcessSystem);
    }
    void GameWindow::Stop()
    {
        World::RemoveSystem(preProcessSystem);
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
            ImGui::Text("No camera direct render to screen.");
        }
        //帧率信息
        static float deltaTime = 0;
        deltaTime = std::lerp(deltaTime, GlobalSceneWindow.GetSceneTimeSystem().GetDeltaTimeReal(), 0.3f);
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