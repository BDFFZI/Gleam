#include "LightGraphics/Runtime/Graphics.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"

using namespace Light;

void DrawUI()
{
    ImGui::ShowDemoWindow();
    ImGui::Image(UI::CreateImage(), float2(50));
}

int main()
{
    Window window = Window::Initialize("Tests", 800, 600, false);
    GL gl = GL::Initialize(Window::GetGlfwWindow());
    Graphics graphics = Graphics::Initialize(gl);
    UI::Initialize(window, graphics);

    Window::SetWindowUpdateEvent([]()
    {
        //逻辑处理
        if (Input::GetKeyDown(KeyCode::Esc))
        {
            Window::Stop();
            return;
        }

        GLCommandBuffer* presentCommandBuffer;
        if (Graphics::BeginPresent(&presentCommandBuffer) == false)
            return;

        CommandBuffer& commandBuffer = Graphics::ApplyCommandBuffer();
        commandBuffer.BeginRecording();
        commandBuffer.BeginRendering(Graphics::GetPresentRenderTarget(), true);

        UI::BeginFrame();
        DrawUI();
        UI::EndFrame(commandBuffer.GetGLCommandBuffer());

        commandBuffer.EndRendering();
        commandBuffer.EndRecording();

        presentCommandBuffer->ExecuteSubCommands(commandBuffer.GetGLCommandBuffer());
        Graphics::EndPresent(*presentCommandBuffer);
        Graphics::WaitPresent();

        Graphics::ReleaseCommandBuffer(commandBuffer);
    });
    Window::Start();

    UI::UnInitialize();
    Graphics::UnInitialize();
    GL::UnInitialize();

    return 0;
}
