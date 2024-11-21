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
#include "Systems/UISystem.h"

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
        World::AddSystem<BeginUISystem>();
        World::AddSystem<EndUISystem>();
        World::AddSystem<BeginPhysicsSystem>();
        World::AddSystem<EndPhysicsSystem>();
        World::AddSystem<RenderingSystem>();
        World::AddSystem<LogicSystem>();

        Entity entities[5][5];
        constexpr int length = 4;
        const int bevelLength = sqrt(pow(length, 2) + pow(length, 2));
        for (int i = 0, y = -10; i < 5; i++, y += length)
            for (int j = 0, x = -10; j < 5; j++, x += length)
            {
                Entity entity = World::AddEntity(massPointArchetype, Point({static_cast<float>(x), static_cast<float>(y)}));
                entities[i][j] = entity;
            }

        for (int y = 0; y < 5; y++)
            for (int x = 0; x < 5; x++)
            {
                if (x + 1 < 5)
                    World::AddEntity(springArchetype, SpringPhysics(entities[y][x], entities[y][x + 1], length));
                if (y + 1 < 5)
                    World::AddEntity(springArchetype, SpringPhysics(entities[y][x], entities[y + 1][x], length));
                if (x + 1 < 5 && y + 1 < 5)
                    World::AddEntity(springArchetype, SpringPhysics(entities[y][x], entities[y + 1][x + 1], bevelLength));
                if (x + 1 < 5 && y - 1 >= 0)
                    World::AddEntity(springArchetype, SpringPhysics(entities[y][x], entities[y - 1][x + 1], bevelLength));
                if (x + 2 < 5)
                    World::AddEntity(springArchetype, SpringPhysics(entities[y][x], entities[y][x + 2], length * 2));
                if (y + 2 < 5)
                    World::AddEntity(springArchetype, SpringPhysics(entities[y][x], entities[y + 2][x], length * 2));
            }
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
