#pragma once
#include "GleamECS/Runtime/Scene.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    /**
     * 场景资源包装器，是场景的部分数据在资源包中的替身，使得场景可以被持久化
     *
     * 注意！该包装器仅包装场景的部分数据，实体数据由EntityAsset包装。
     */
    class SceneAsset
    {
    public:
        /**
         * 将Scene数据拷贝到AssetBundle，以便用其持久化保存Scene。
         * @param scene 
         * @param assetBundle 
         */
        static void ToAssetBundle(Scene& scene, AssetBundle& assetBundle);
        /**
         * 从AssetBundle中复原Scene并夺取相关资源的所有权。
         * @param assetBundle
         * @param isRunning 
         * @return 
         */
        static Scene& FromAssetBundle(AssetBundle& assetBundle, bool isRunning = false);

    private:
        Gleam_MakeType_Friend
        std::string name;
        std::vector<uuids::uuid> systems;
    };
    Gleam_MakeType(SceneAsset, "3CA95E07-FCD9-4DCE-ABE3-6115152EA9D7")
    {
        Gleam_MakeType_AddField(name);
        Gleam_MakeType_AddField(systems);
    }
}