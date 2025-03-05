#include "GleamEngine/Runtime/Engine.h"
#include "GleamScene/Runtime/Scene.h"

using namespace Gleam;

struct Component1
{
};

class System1 : public System
{
};
System1 system1 = Engine::CreateSystem<System1>();

Gleam_MakeInitEvent()
{
    Scene& scene = Scene::Create("TestScene");
    Entity entity1 = World::AddEntity(Component1{});
    World::AddSystem(system1);
    scene.AddEntity(entity1);
    scene.AddSystem(system1);
}

Gleam_Main