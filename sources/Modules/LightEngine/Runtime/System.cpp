#include "System.h"

namespace Light
{
    void AwakeSystem::Update()
    {
        Awake->MoveAllEntities(World::GetMainScene());
    }

    void DestroySystem::Update()
    {
        Destroy->RemoveAllEntities();
    }
}