#include "GameWindow.h"

#include <imgui_impl_vulkan.h>

#include "SceneWindow.h"
#include "GleamECS/Runtime/View.h"
#include "GleamECS/Runtime/World/World.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamGraphics/Runtime/SwapChain.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    void GameWindow::Start()
    {
        lastImageSize = 0; //以便重启时能触发纹理重建
        preProcessSystem.OnUpdate() = [this]
        {
            //重建渲染目标和纹理
            if (isDirty && lastImageSize.x > 0 && lastImageSize.y > 0)
            {
                isDirty = false;
                SwapChain::WaitPresent();
                if (renderTextureID != nullptr)
                    UI::DeleteTexture(renderTextureID);
                renderTexture = std::make_unique<GRenderTexture>(static_cast<int2>(lastImageSize));
                renderTextureID = UI::CreateTexture(*renderTexture);
                GlobalRenderingSystem.SetDefaultRenderTarget(*renderTexture);
            }
            //更新输入系统的焦点范围为GameWindow
            GlobalInputSystem.SetFocusArea(Rectangle::CreateFromOrigin(imagePosition, lastImageSize));
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
        if (ImGui::Begin("GameWindow", nullptr, ImGuiWindowFlags_MenuBar))
        {
            //获取窗口信息
            float2 imageSize = ImGui::GetContentRegionAvail();
            if (!equal(aspect, 0.0f))
            {
                if (imageSize.x / imageSize.y > aspect)
                    imageSize.x = imageSize.y * aspect;
                else
                    imageSize.y = imageSize.x / aspect;
            }
            imagePosition = ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail() / 2 - imageSize / 2;
            //判断重建纹理
            if (any(imageSize != lastImageSize))
                isDirty = true;
            lastImageSize = imageSize;
            //游戏窗口菜单
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Resolution"))
                {
                    ImGui::DragFloat("Aspect", &aspect, 0.1f, 0, 2);
                    if (ImGui::Button("FreeAspect"))
                        aspect = 0;
                    if (ImGui::Button("1920x1080"))
                        aspect = 1920.0f / 1080.0f;
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            //显示游戏画面
            int cameraCount = 0;
            View<Camera>::Each([&cameraCount](Camera& camera)
            {
                if (camera.renderTarget == std::nullopt)
                    cameraCount++;
            });
            if (cameraCount != 0)
            {
                ImGui::SetCursorScreenPos(imagePosition);
                if (renderTextureID != nullptr)
                    ImGui::Image(renderTextureID, imageSize);
            }
            else
            {
                ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y / 2);
                ImGui::Text("No camera direct render to screen.");
            }
        }
        ImGui::End();
    }
}