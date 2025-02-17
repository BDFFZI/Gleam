#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamMath/Runtime/Geometry/Solid/Cuboid.h"
#include "GleamMath/Runtime/Geometry/Solid/Sphere.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

namespace Gleam
{
    class Gizmos : public System
    {
    public:
        static Gizmos& GetInstance();

        static float4x4& LocalToWorld();
        static void DrawCuboid(Cuboid cuboid);
        static void DrawSphere(Sphere sphere);

    private:
        Gleam_Engine_Friend
        
        inline static float4x4 localToWorld = float4x4::Identity();
        inline static std::unique_ptr<GShader> lineMeshShader = nullptr;
        inline static std::unique_ptr<Material> lineMeshMaterial = nullptr;
        inline static std::unique_ptr<Mesh> cubeMesh;
        inline static std::unique_ptr<Mesh> sphereMesh;

        Gizmos(): System(RenderingSystem, OrderRelation::Before)
        {
        }

        void Start() override;
        void Stop() override;
        void Update() override;
    };
}