#include "Archetype.h"
#include "FixedPointSystem.h"
#include "LineUpdateSystem.h"
#include "LogicSystem.h"
#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightUI/Runtime/UI.h"
#include "Physics/CollisionSystem.h"
#include "Physics/ForceSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/PositionSystem.h"

using namespace Light;

int main()
{
    //添加系统
    World::AddSystems({&PhysicsSystem, &ForceSystem, &PositionSystem, &CollisionSystem});
    World::AddSystems({&FixedPointSystem, &LineUpdateSystem, &LogicSystem});
    //添加实体
    Entity entities[5][5];
    constexpr int length = 4;
    const float bevelLength = static_cast<float>(sqrt(pow(length, 2) + pow(length, 2)));
    for (int i = 0, y = -10; i < 5; i++, y += length)
        for (int j = 0, x = -10; j < 5; j++, x += length)
        {
            const Entity entity = World::AddEntity(MassPointArchetype);
            World::SetComponents(entity, Point{{static_cast<float>(x), static_cast<float>(y)}});
            entities[i][j] = entity;
        }
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
        {
            if (x + 1 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{entities[y][x], entities[y][x + 1], length});
            }
            if (y + 1 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{entities[y][x], entities[y + 1][x], length});
            }
            if (x + 1 < 5 && y + 1 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{entities[y][x], entities[y + 1][x + 1], bevelLength});
            }
            if (x + 1 < 5 && y - 1 >= 0)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{entities[y][x], entities[y - 1][x + 1], bevelLength});
            }
            if (x + 2 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{entities[y][x], entities[y][x + 2], length * 2});
            }
            if (y + 2 < 5)
            {
                Entity entity = World::AddEntity(SpringArchetype);
                World::SetComponents(entity, SpringPhysics{entities[y][x], entities[y + 2][x], length * 2});
            }
        }

    Engine::Start();
    return 0;
}