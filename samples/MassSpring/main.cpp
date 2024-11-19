#include "System.hpp"
#include "LightECS/Runtime/World.h"
#include "LightGL/Runtime/GL.h"
#include "LightGraphics/Runtime/Graphics.h"
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
        World::AddSystem<PaintSystem>();
        World::AddSystem<LogicSystem>();
        World::AddSystem<PhysicsSystem>();

        Entity point1 = World::AddEntity(massPointArchetype, Point({-0.5f, 0}));
        Entity point2 = World::AddEntity(massPointArchetype, Point({0.5f, 0}));
        Entity line2 = World::AddEntity(springArchetype, SpringPhysics(point1, point2));
    });
    Window::SetWindowUpdateEvent([]()
    {
        World::Update();
    });
    Window::SetWindowStopEvent([]
    {
        World::Clear();
    });
    Window::Start();

    UI::UnInitialize();
    Graphics::UnInitialize();
    GL::UnInitialize();

    return 0;
}
