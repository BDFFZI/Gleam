#include <imgui.h>

#include "LightGL/Runtime/GL.h"
#include "LightGraphics/Runtime/CommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"

using namespace Light;

void Start()
{
}

void LogicUpdate()
{
    //逻辑处理
    if (Input::GetKeyDown(KeyCode::Esc))
    {
        Window::Stop();
    }
}

void RenderUpdate(CommandBuffer& commandBuffer)
{
    commandBuffer.
}

void UIUpdate()
{
    ImGui::ShowDemoWindow();
}

void ImageUpdate()
{
    CommandBuffer& commandBuffer = Graphics::GetCommandBuffer();
    commandBuffer.BeginRecording();
    commandBuffer.BeginRendering(nullptr);

    RenderUpdate();

    UI::BeginFrame();
    UIUpdate();
    UI::EndFrame(commandBuffer);

    commandBuffer.EndRendering();
    commandBuffer.EndRecording();
    Graphics::PresentAsync(commandBuffer);
    Graphics::WaitPresent();
    Graphics::ReleaseCommandBuffer(commandBuffer);
}

int main()
{
    Window::Initialize("MassSpring");
    GL gl = GL::Initialize(Window::GetGlfwWindow());
    Graphics graphics = Graphics::Initialize(gl);
    UI::Initialize(graphics);

    Window::SetWindowStartEvent(Start);
    Window::SetWindowUpdateEvent([]()
    {
        LogicUpdate();
        ImageUpdate();
    });
    Window::Start();

    UI::UnInitialize();
    Graphics::UnInitialize();
    GL::UnInitialize();

    return 0;
}
