#pragma once
#include "GleamECS/Runtime/Scene.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"
#include "Scene/SceneAsset.h"

namespace Gleam
{
    /**
     * 支持通过资源包加载、切换场景，并负责这些场景的回收工作
     *
     * 注意！编辑器时或运行时加载场景都应通过该接口，因为场景本身是不托管资源包信息的，因此无法处理第三方加载的场景。
     */
    class SceneManager
    {
    public:
        static void LoadScene(const uuids::uuid assetBundleID)
        {
            AssetBundle& assetBundle = Resources::Load(assetBundleID);
            Scene& scene = SceneAsset::FromAssetBundle(assetBundle);
            scenes.emplace(assetBundle.GetID(), &scene);

#ifdef GleamEngineEditor
            if (Editor::IsPlaying())
#endif
            scene.Start();
        }
        static void UnloadScene(const uuids::uuid assetBundleID)
        {
            stoppingScenes.push_back(assetBundleID);
        }

    private:
        friend void SceneManager_FlushUnloadingScenes();
        friend class SceneManager_RuntimeEvent;

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

    /**
     * 辅助SceneManager在引擎运行的部分时间段执行一些额外处理
     */
    class SceneManager_RuntimeEvent : public System
    {
    public:
        SceneManager_RuntimeEvent(): System(GlobalPreUpdateSystem, MinOrder, MinOrder)
        {
        }

    private:
        void Start() override;
        void Stop() override;
    };
    Gleam_MakeGlobalSystem(SceneManager_RuntimeEvent)
}