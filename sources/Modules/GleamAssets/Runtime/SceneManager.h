#pragma once
#include "GleamAssets/Runtime/Scene/Scene.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    class SceneManager
    {
    public:
        static void LoadScene(uuids::uuid assetBundleID);
        static void UnloadScene(uuids::uuid assetBundleID);

    private:
        friend void SceneManager_FlushUnloadingScenes();
        inline static std::unordered_map<uuids::uuid, Scene*> scenes;
        inline static std::vector<uuids::uuid> stoppingScenes;
        inline static std::vector<uuids::uuid> destroyingScenes;
    };

    /**
     * 停止Scene后等待1帧再实际销毁，从而使Scene中的System能有时间处理Scene中的Entity。
     * 
     * 当引擎停止时，由于没有事件通知（有也没用，因为该函数要延迟1帧销毁，但引擎立即就要停止了），该函数不会执行。
     * 但无妨，因为World.Clear()也会实现相同的功能，而且避免了World和Scene重复销毁Entity的问题。
     */
    void SceneManager_FlushUnloadingScenes();
}