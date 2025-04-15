#include "GameWindow.h"

#include <imgui_impl_vulkan.h>

#include "SceneWindow.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamGraphics/Runtime/SwapChain.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    void GameWindow_PreProcess::Start()
    {
        gameWindow = &World::GetSystemAllocator().GetSystem<GameWindow>();
        renderingSystem = &World::GetSystemAllocator().GetSystem<RenderingSystem>();
    }
    void GameWindow_PreProcess::Update()
    {
        //重建渲染目标和纹理
        if (gameWindow->isDirty && gameWindow->lastImageSize.x > 0 && gameWindow->lastImageSize.y > 0)
        {
            gameWindow->isDirty = false;
            SwapChain::WaitPresent();
            if (gameWindow->renderTextureID != nullptr)
                UI::DeleteTexture(gameWindow->renderTextureID);
            gameWindow->renderTexture = std::make_shared<GRenderTexture>(static_cast<int2>(gameWindow->lastImageSize));
            gameWindow->renderTextureID = UI::CreateTexture(*gameWindow->renderTexture);
            renderingSystem->SetDefaultRenderTarget(*gameWindow->renderTexture);
        }
        //更新输入系统的焦点范围为GameWindow
        if (InputSystem* inputSystem = World::GetSystemAllocator().TryGetSystem<InputSystem>())
            inputSystem->SetFocusArea(Rectangle::CreateFromOrigin(gameWindow->imagePosition, gameWindow->lastImageSize));
    }

    void GameWindow::Stop()
    {
        //以便重启时能触发纹理重建
        lastImageSize = 0;
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
            //显示游戏画面（必须保证在相机渲染后才可显示游戏画面。例如，假设ProjectWindow在GameWindow前执行，由于其加载场景是立即的，导致相机未渲染时被GameWindow窗口捕获，于是就会触发vk验证错误）
            int cameraCount = 0;
            World::GetView<Camera>().Each([&cameraCount](Camera& camera)
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