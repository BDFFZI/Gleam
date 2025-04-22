#pragma once
#include "Asset/EntityAsset.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamPersistence/Runtime/AssetBundle/AssetBundle.h"

namespace Gleam
{
    /**
     * 场景资源包用于实现运行时场景与持久化资源包之间的相互转换，能自动处理场景间依赖以及实例化等需求，等价于Unity中的普通场景和预制体功能。
     */
    class SceneAssetBundle
    {
    public:
        static void GetSceneAssets(AssetBundle& assetBundle, std::string& outName, std::vector<const SystemInfo*>& outSystems, std::vector<EntityAsset*>& outEntities);
        /**
         * 将Scene中数据的数据映射成AssetBundle，以便用其持久化保存Scene。
         * 注意！该资源包不托管场景对象，场景对象仍属于场景。
         * @param scene 
         * @param assetBundle 
         */
        static void MapToAssetBundle(Scene& scene, AssetBundle& assetBundle);
        /**
         * 从AssetBundle中复原Scene并夺取相关资源的所有权。
         * @param assetBundle
         * @param isRunning 
         * @return 
         */
        static Scene& MoveFromAssetBundle(AssetBundle& assetBundle, bool isRunning = false);
        /**
         * 从资源包中复制相关数据，创建一个新的独立的场景
         * @param assetBundle 
         * @param isRunning 
         * @return 
         */
        static Scene& CopyFromAssetBundle(AssetBundle& assetBundle, bool isRunning = false);
    };
}