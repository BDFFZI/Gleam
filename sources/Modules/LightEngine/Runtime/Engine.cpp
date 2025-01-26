#include "Engine.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    void Engine::Start()
    {
        World::Start();
        while (isStopping == false)
        {
            World::Update();
        }

        //删除所有实体
        for (auto& scene : World::GetAllScenes())
        {
            if (scene.get() != Destroy)
                scene->MoveAllEntities(Destroy);
        }
        World::Update();

        World::Stop();
    }
    void Engine::Stop()
    {
        isStopping = true;
    }
}