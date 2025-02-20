#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"
#include "GleamMath/Runtime/Geometry/3D/Cuboid.h"
#include "GleamMath/Runtime/Geometry/3D/Line.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

namespace Gleam
{
    class Gizmos
    {
    public:
        static void Init();
        static void UnInit();

        static void PushLocalToWorld(const float4x4& localToWorld);
        static void PopLocalToWorld();

        static void DrawPoint(const Point& point, const float4& color = float4::White());
        static void DrawPoint(const float3& point, const float4& color = float4::White());
        static void DrawSegment(const Segment& segment, const float4& color = float4::White());
        static void DrawCuboid(const Cuboid& cuboid, const float4& color = float4::White());
        static void DrawSphere(const Sphere& sphere, const float4& color = float4::White());
        static void DrawWireCuboid(const Cuboid& cuboid, const float4& color = float4::White());
        static void DrawWireSphere(const Sphere& sphere, const float4& color = float4::White());

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
        inline static std::vector<float4x4> localToWorlds = {float4x4::Identity()};

        inline static GizmoQueue cuboidQueue = {};
        inline static GizmoQueue sphereQueue = {};
        inline static GizmoQueue wireCuboidQueue = {};
        inline static GizmoQueue wireSphereQueue = {};
        inline static std::unique_ptr<Mesh> pointsMesh = nullptr;
        inline static std::unique_ptr<Material> pointsMaterial = nullptr;
        inline static std::unique_ptr<Mesh> linesMesh = nullptr;
        inline static std::unique_ptr<Material> linesMaterial = nullptr;

        static void Draw(GizmoQueue& instanceQueue, Mesh& mesh);
    };


}