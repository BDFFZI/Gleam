#pragma once
#include "LightECS/Runtime/Scene.h"
#include "LightECS/Runtime/System.h"
#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/Engine.h"

namespace Light
{
    class AwakeSystem : public System
    {
    public:
        AwakeSystem(): System(std::nullopt, RightOrder, RightOrder)
        {
        }

    private:
        void Update() override
        {
            Awake->MoveAllEntities(World::GetMainScene());
        }
    };
    Light_MakeSystem(AwakeSystem)
}