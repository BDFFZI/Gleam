#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamMath/Runtime/Geometry/Solid/Cuboid.h"
#include "GleamMath/Runtime/Geometry/Solid/Sphere.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

namespace Gleam
{
    class Gizmos
    {
    public:
        static void Init();
        static void UnInit();

        static float4x4& LocalToWorld();
        static float4& Color();

        static void DrawCuboid(const Cuboid& cuboid);
        static void DrawSphere(const Sphere& sphere);
        static void DrawWireCuboid(const Cuboid& cuboid);
        static void DrawWireSphere(const Sphere& sphere);

    private:
        friend class GizmosSystem;

        struct GizmoInfo
        {
            float4x4 localToWorld;
            float4 albedo;
        };

        struct GizmoQueue
        {
            std::unique_ptr<Material> material = nullptr;
            std::unique_ptr<GBuffer> buffer = nullptr;
            std::vector<GizmoInfo> instances = {};

            void Flush();
            void Clear();
            void Destroy();
        };

        inline static std::unique_ptr<GSAssetLayout> assetLayout = nullptr;
        inline static std::unique_ptr<GShader> gizmoShader = nullptr;
        inline static std::unique_ptr<GShader> wireGizmoShader = nullptr;
        inline static std::unique_ptr<Mesh> cubeMesh = nullptr;
        inline static std::unique_ptr<Mesh> sphereMesh = nullptr;
        inline static float4x4 localToWorld = float4x4::Identity();
        inline static float4 color = float4::White();

        inline static GizmoQueue cuboidQueue = {};
        inline static GizmoQueue sphereQueue = {};
        inline static GizmoQueue wireCuboidQueue = {};
        inline static GizmoQueue wireSphereQueue = {};

        static void Draw(GizmoQueue& instanceQueue, Mesh& mesh);
    };

    class GizmosSystem : public System
    {
    public:
        static GizmosSystem& GetInstance();

    private:
        Gleam_Engine_Friend

        GizmosSystem(): System(RenderingSystem, OrderRelation::Before)
        {
        }
        void Update() override;
    };
}