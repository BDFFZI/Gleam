#include "FixedPointSystem.h"
#include "GameUISystem.h"
#include "LineUpdateSystem.h"
#include "Editor/GameWindow.h"
#include "Editor/HierarchyWindow.h"
#include "Editor/InspectorWindow.h"
#include "LightECS/Runtime/World.h"
#include "LightGL/Runtime/GL.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Window.h"
#include "Physics/CollisionSystem.h"
#include "Physics/ForceSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/PositionSystem.h"

using namespace Light;

int main()
{
    Window window = Window::Initialize("MassSpring", static_cast<int>(1920 * 0.7f), static_cast<int>(1080 * 0.7f), false);
    std::vector<const char*> extensions;
    Graphics::InitializeGLDemand(extensions);
    GL gl = GL::Initialize(Window::GetGlfwWindow(), extensions);
    Graphics graphics = Graphics::Initialize(gl);
    UI::Initialize(window, graphics);

    Window::SetWindowStartEvent([]
    {
        World::AddSystem({PhysicsSystem, ForceSystem, PositionSystem, CollisionSystem});
        World::AddSystem({UISystem, GameUISystem});
        World::AddSystem({FixedPointSystem, LineUpdateSystem,})
        World::AddSystem<EndPresentationSystem>();
        World::AddSystem<BeginUISystem>();
        World::AddSystem<EndUISystem>();
        World::AddSystem<BeginPhysicsSystem>();
        World::AddSystem<EndPhysicsSystem>();
        World::AddSystem<RenderingSystem>();
        World::AddSystem<LogicSystem>();
        World::AddSystem<HierarchyWindow>();
        World::AddSystem<InspectorWindow>();
        World::AddSystem<GameWindow>();

        Entity entities[5][5];
        constexpr int length = 4;
        float bevelLength = static_cast<float>(sqrt(pow(length, 2) + pow(length, 2)));
        for (int i = 0, y = -10; i < 5; i++, y += length)
            for (int j = 0, x = -10; j < 5; j++, x += length)
            {
                Entity entity = World::AddEntity(massPointArchetype, Point{{static_cast<float>(x), static_cast<float>(y)}});
                entities[i][j] = entity;
            }

        for (int y = 0; y < 5; y++)
            for (int x = 0; x < 5; x++)
            {
                if (x + 1 < 5)
                    World::AddEntity(springArchetype, SpringPhysics{entities[y][x], entities[y][x + 1], length});
                if (y + 1 < 5)
                    World::AddEntity(springArchetype, SpringPhysics{entities[y][x], entities[y + 1][x], length});
                if (x + 1 < 5 && y + 1 < 5)
                    World::AddEntity(springArchetype, SpringPhysics{entities[y][x], entities[y + 1][x + 1], bevelLength});
                if (x + 1 < 5 && y - 1 >= 0)
                    World::AddEntity(springArchetype, SpringPhysics{entities[y][x], entities[y - 1][x + 1], bevelLength});
                if (x + 2 < 5)
                    World::AddEntity(springArchetype, SpringPhysics{entities[y][x], entities[y][x + 2], length * 2});
                if (y + 2 < 5)
                    World::AddEntity(springArchetype, SpringPhysics{entities[y][x], entities[y + 2][x], length * 2});
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
