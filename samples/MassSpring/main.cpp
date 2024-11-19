#include "Component.hpp"
#include "LightECS/Runtime/World.h"
#include "LightGL/Runtime/GL.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Time.h"
#include "LightWindow/Runtime/Window.h"
#include "Systems/LogicSystem.h"
#include "Systems/PresentationSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/RenderingSystem.h"

using namespace Light;

int main()
{
    Window window = Window::Initialize("MassSpring", 800, 800);
    std::vector<const char*> extensions;
    Graphics::InitializeGLDemand(extensions);
    GL gl = GL::Initialize(Window::GetGlfwWindow(), extensions);
    Graphics graphics = Graphics::Initialize(gl);
    UI::Initialize(window, graphics);

    Window::SetWindowStartEvent([]
    {
        World::AddSystem<BeginSimulationSystem>();
        World::AddSystem<EndSimulationSystem>();
        World::AddSystem<BeginPresentationSystem>();
        World::AddSystem<EndPresentationSystem>();
        World::AddSystem<LogicSystem>();
        World::AddSystem<PhysicsSystem>();
        World::AddSystem<RenderingSystem>();

        Entity point1 = World::AddEntity(massPointArchetype, Point({-10, 0}));
        Entity point2 = World::AddEntity(massPointArchetype, Point({10, 0}));
        Entity line2 = World::AddEntity(springArchetype, SpringPhysics(point1, point2));
    });
    Window::SetWindowUpdateEvent([]()
    {
        World::Update();

        if (Input::GetKey(KeyCode::Space))
            Time::SetTimeScale(1);
        else
            Time::SetTimeScale(0);
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
