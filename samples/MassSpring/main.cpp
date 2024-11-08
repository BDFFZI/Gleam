#include <imgui.h>

#include "LightGL/Runtime/GL.h"
#include "LightGraphics/Runtime/CommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightGraphics/Runtime/Shader.h"
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
    commandBuffer.ClearRenderTexture(float4(0, 0, 1, 1));
}

void UIUpdate()
{
    ImGui::ShowDemoWindow();
}

void ImageUpdate()
{
    CommandBuffer& commandBuffer = Graphics::GetCommandBuffer();
    commandBuffer.BeginRecording();
    commandBuffer.BeginRendering(Graphics::GetPresentRenderTexture());

    RenderUpdate(commandBuffer);

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
    std::vector<const char*> extensions;
    Graphics::InitializeGLDemand(extensions);
    GL gl = GL::Initialize(Window::GetGlfwWindow(), extensions);
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
