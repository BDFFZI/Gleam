#include "Archetype.h"
#include "FixedPointSystem.h"
#include "GameUISystem.h"
#include "LineUpdateSystem.h"
#include "LogicSystem.h"
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

    static std::initializer_list<System*> gameLogics = {&FixedPointSystem, &LineUpdateSystem, &LogicSystem};
    static std::initializer_list<System*> editorWindows = {&GameWindow, &GameWindowAssetsSystem, &HierarchyWindow, &InspectorWindow};

    Window::SetWindowStartEvent([]
    {
        //添加系统
        World::AddSystems({&PhysicsSystem, &ForceSystem, &PositionSystem, &CollisionSystem});
        World::AddSystems({&RenderingSystem,});
        World::AddSystems({&UISystem, &GameUISystem});
        World::AddSystems(gameLogics);
        World::AddSystems(editorWindows);
        //添加实体
        Entity entities[5][5];
        constexpr int length = 4;
        const float bevelLength = static_cast<float>(sqrt(pow(length, 2) + pow(length, 2)));
        for (int i = 0, y = -10; i < 5; i++, y += length)
            for (int j = 0, x = -10; j < 5; j++, x += length)
            {
                const Entity entity = World::AddEntity(MassPointArchetype, Point{{static_cast<float>(x), static_cast<float>(y)}});
                entities[i][j] = entity;
            }
        for (int y = 0; y < 5; y++)
            for (int x = 0; x < 5; x++)
            {
                if (x + 1 < 5)
                    World::AddEntity(SpringArchetype, SpringPhysics{entities[y][x], entities[y][x + 1], length});
                if (y + 1 < 5)
                    World::AddEntity(SpringArchetype, SpringPhysics{entities[y][x], entities[y + 1][x], length});
                if (x + 1 < 5 && y + 1 < 5)
                    World::AddEntity(SpringArchetype, SpringPhysics{entities[y][x], entities[y + 1][x + 1], bevelLength});
                if (x + 1 < 5 && y - 1 >= 0)
                    World::AddEntity(SpringArchetype, SpringPhysics{entities[y][x], entities[y - 1][x + 1], bevelLength});
                if (x + 2 < 5)
                    World::AddEntity(SpringArchetype, SpringPhysics{entities[y][x], entities[y][x + 2], length * 2});
                if (y + 2 < 5)
                    World::AddEntity(SpringArchetype, SpringPhysics{entities[y][x], entities[y + 2][x], length * 2});
            }

        World::Start();
    });
    Window::SetWindowUpdateEvent([]()
    {
        World::Update();
    });
    Window::SetWindowStopEvent([]
    {
        World::Stop();
    });
    Window::Start();

    UI::UnInitialize();
    Graphics::UnInitialize();
    GL::UnInitialize();

    return 0;
}