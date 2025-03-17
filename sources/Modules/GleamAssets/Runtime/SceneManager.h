#pragma once
#include "GleamECS/Runtime/Scene.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"
#include "SceneAsset/SceneAsset.h"

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
        static Scene& LoadScene(uuids::uuid assetBundleID, bool start = true);
        static void UnloadScene(uuids::uuid assetBundleID);
        /**
         * 立即卸载场景。
         *
         * 注意！这种方式无法保证系统停止事件在实体回收前触发。
         * @param assetBundleID 
         */
        static void UnloadSceneImmediate(uuids::uuid assetBundleID);
        static void UnloadSceneImmediate(Scene& scene);

    private:
        friend void SceneManager_FlushUnloadingScenes();
        friend class SceneManager_RuntimeEvent;

        inline static std::unordered_map<uuids::uuid, Scene*> allScenes;
        inline static std::vector<uuids::uuid> stoppingScenes;
        inline static std::vector<uuids::uuid> destroyingScenes;
    };

    /**
     * 停止Scene后需等待1帧再实际销毁，从而使Scene中的System能有时间处理Scene中的Entity。
     * 
     * 当引擎停止时，由于没有事件通知（有也没用，因为该函数要延迟1帧销毁，但引擎立即就要停止了），该函数不会执行。
     * 但无妨，因为World.Clear()也会实现相同的功能，而且避免了World和Scene重复销毁Entity的问题。
     */
    void SceneManager_FlushUnloadingScenes();

    /**
     * 引擎停止时，需释放场景（世界负责回收，场景需释放所有权）并回收资源包
     */
    class SceneManager_RuntimeEvent : public System
    {
    public:
        SceneManager_RuntimeEvent(): System(GlobalPreUpdateSystem, MinOrder, MinOrder)
        {
        }

    private:
        void Stop() override;
    };
    Gleam_MakeGlobalSystem(SceneManager_RuntimeEvent)
}