#pragma once
#include "GleamECS/Runtime/Scene.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"
#include "Asset/BasicSceneInfo.h"

namespace Gleam
{
    /**
     * 支持通过资源包加载、切换场景，并负责这些场景的回收工作
     *
     * 注意！编辑器时或运行时加载场景都应通过该接口，因为场景本身是不托管资源包信息的，因此SceneManager无法处理第三方加载的场景。
     */
    class SceneManager
    {
    public:
        static auto GetAllScenes()
        {
            return allScenes | std::views::values | std::views::transform(
                [](auto* scene) { return std::reference_wrapper<Scene>(*scene); }
            );
        }

        static bool HasScene(uuids::uuid assetBundleID);
        static Scene& LoadScene(uuids::uuid assetBundleID, bool isRunning = true);
        static std::optional<std::reference_wrapper<Scene>> LoadScene(std::string_view sceneName, bool isRunning = true);
        static void UnloadScene(uuids::uuid assetBundleID);

    private:
        friend void SceneManager_ReleaseScenes();

        inline static std::unordered_map<uuids::uuid, Scene*> allScenes;
    };

    //引擎停止时，需释放场景（世界负责回收，场景需释放所有权）并回收资源包
    void SceneManager_ReleaseScenes();
    Gleam_MakeRuntimeSystemEvent(SceneManager_ReleaseScenes, Stop, GlobalPostUpdateSystem, System::MaxOrder)
}