#include "Component.hpp"
#include "LightECS/Runtime/World.h"
#include "LightGL/Runtime/GL.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Window.h"
#include "Systems/LogicSystem.h"
#include "Systems/PresentationSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/RenderingSystem.h"

using namespace Light;

int main()
{
    Window window = Window::Initialize("MassSpring", 800, 800, false);
    std::vector<const char*> extensions;
    Graphics::InitializeGLDemand(extensions);
    GL gl = GL::Initialize(Window::GetGlfwWindow(), extensions);
    Graphics graphics = Graphics::Initialize(gl);
    UI::Initialize(window, graphics);

    Window::SetWindowStartEvent([]
    {
        World::AddSystem<BeginPresentationSystem>();
        World::AddSystem<EndPresentationSystem>();
        World::AddSystem<BeginPhysicsSystem>();
        World::AddSystem<EndPhysicsSystem>();
        World::AddSystem<RenderingSystem>();
        World::AddSystem<LogicSystem>();

        Entity point1 = World::AddEntity(massPointArchetype, Point({-10, 0}));
        Entity point2 = World::AddEntity(massPointArchetype, Point({10, 0}));
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
