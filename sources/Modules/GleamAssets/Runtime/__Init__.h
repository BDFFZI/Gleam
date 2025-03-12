#pragma once
#include <iostream>

#include "SceneManager.h"
#include "Scene/EntityTransferrer.h"

namespace Gleam
{
    Gleam_AddEngineUpdateEvent(SceneManager_FlushUnloadingScenes, 0)

    //显式特化，避免被编辑器无视
    inline auto TransferEntity = FieldDataTransferrer_Transfer<Entity>::Invoke;

    Gleam_MakeInitEvent()
    {
        std::cout << typeid(TransferEntity).name() << std::endl;
    }
}