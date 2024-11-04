#include <imgui.h>

#include "LightGraphics/Runtime/Graphics.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"

void main()
{
    using namespace Light;

    Window::Initialize("Tests");
    GL gl = GL::Initialize(Window::GetGlfwWindow());
    Graphics graphics = Graphics::Initialize(gl);
    UI::Initialize(graphics);

    Window::SetWindowUpdateEvent([]()
    {
        //逻辑处理
        if (Input::GetKeyDown(KeyCode::Esc))
        {
            Window::Stop();
            return;
        }

        CommandBuffer& commandBuffer = Graphics::GetCommandBuffer();
        commandBuffer.BeginRecording();
        commandBuffer.BeginRendering(nullptr);

        UI::BeginFrame();
        ImGui::ShowDemoWindow();
        UI::EndFrame(commandBuffer);

        commandBuffer.EndRendering();
        commandBuffer.EndRecording();
        Graphics::PresentAsync(commandBuffer);
        Graphics::WaitPresent();
        Graphics::ReleaseCommandBuffer(commandBuffer);
    });
    Window::Start();

    UI::UnInitialize();
    Graphics::UnInitialize();
    GL::UnInitialize();
}
