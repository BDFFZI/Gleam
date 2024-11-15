#include <imgui.h>

#include "System.hpp"
#include "LightECS/Runtime/World.h"
#include "LightGL/Runtime/GL.h"
#include "LightGraphics/Runtime/CommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightGraphics/Runtime/Shader.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"

using namespace Light;

int main()
{
    Window::Initialize("MassSpring");
    std::vector<const char*> extensions;
    Graphics::InitializeGLDemand(extensions);
    GL gl = GL::Initialize(Window::GetGlfwWindow(), extensions);
    Graphics graphics = Graphics::Initialize(gl);
    UI::Initialize(graphics);

    Window::SetWindowStartEvent([]
    {
        World::AddSystem<BeginPaintSystem>();
        World::AddSystem<EndPaintSystem>();
    });
    Window::SetWindowUpdateEvent([]()
    {
        World::Update();
    });
    Window::Start();

    UI::UnInitialize();
    Graphics::UnInitialize();
    GL::UnInitialize();

    return 0;
}
