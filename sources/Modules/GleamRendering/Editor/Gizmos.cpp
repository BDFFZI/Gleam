#include "Gizmos.h"

#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Rendering.h"

struct InstanceInfo;

namespace Gleam
{
    void Gizmos::Init()
    {
        GSCodeLayout codeLayout = GSCodeLayout{"Resources/GleamRenderingEditor/Gizmo.hlsl"};
        GSStateLayout stateLayout = Graphics::GetGraphicsConfig().defaultGSStateLayout;
        std::vector<GLDescriptorBinding> descriptorBindings = RenderingConfig::DescriptorBindings;
        descriptorBindings.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        assetLayout = std::make_unique<GSAssetLayout>(descriptorBindings, RenderingConfig::PushConstantRanges);
        gizmoShader = std::make_unique<GShader>(codeLayout, stateLayout, *assetLayout);
        stateLayout.rasterization.polygonMode = VK_POLYGON_MODE_LINE;
        wireGizmoShader = std::make_unique<GShader>(codeLayout, stateLayout, *assetLayout);

        cubeMesh = std::make_unique<Mesh>(ModelImporter::ImportObj("Resources/GleamRenderingEditor/Cube.obj"));
        sphereMesh = std::make_unique<Mesh>(ModelImporter::ImportObj("Resources/GleamRenderingEditor/Sphere.obj"));

        cuboidQueue = {std::make_unique<Material>(*gizmoShader, *assetLayout)};
        sphereQueue = {std::make_unique<Material>(*gizmoShader, *assetLayout)};
        wireCuboidQueue = {std::make_unique<Material>(*wireGizmoShader, *assetLayout)};
        wireSphereQueue = {std::make_unique<Material>(*wireGizmoShader, *assetLayout)};
    }
    void Gizmos::UnInit()
    {
        assetLayout.reset();
        gizmoShader.reset();
        wireGizmoShader.reset();
        cubeMesh.reset();
        sphereMesh.reset();
        cuboidQueue.Destroy();
        sphereQueue.Destroy();
        wireCuboidQueue.Destroy();
        wireSphereQueue.Destroy();
    }
    float4x4& Gizmos::LocalToWorld()
    {
        return localToWorld;
    }
    float4& Gizmos::Color()
    {
        return color;
    }

    void Gizmos::DrawCuboid(const Cuboid& cuboid)
    {
        cuboidQueue.instances.emplace_back(
            mul(localToWorld, float4x4::TRS(cuboid.GetCenter(), float3{0}, cuboid.GetSize())),
            color
        );
    }
    void Gizmos::DrawSphere(const Sphere& sphere)
    {
        sphereQueue.instances.emplace_back(
            mul(localToWorld, float4x4::TRS(sphere.GetCenter(), float3{0}, sphere.GetRadius())),
            color
        );
    }
    void Gizmos::DrawWireCuboid(const Cuboid& cuboid)
    {
        wireCuboidQueue.instances.emplace_back(
            mul(localToWorld, float4x4::TRS(cuboid.GetCenter(), float3{0}, cuboid.GetSize())),
            color
        );
    }
    void Gizmos::DrawWireSphere(const Sphere& sphere)
    {
        wireSphereQueue.instances.emplace_back(
            mul(localToWorld, float4x4::TRS(sphere.GetCenter(), float3{0}, sphere.GetRadius())),
            color
        );
    }

    void Gizmos::GizmoQueue::Flush()
    {
        if (buffer == nullptr || buffer->GetSize() < sizeof(GizmoInfo) * instances.size())
        {
            buffer = std::make_unique<GBuffer>(sizeof(GizmoInfo) * instances.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
            material->SetBuffer(5, *buffer);
        }
        buffer->SetData(instances.data());
    }
    void Gizmos::GizmoQueue::Clear()
    {
        instances.clear();
    }
    void Gizmos::GizmoQueue::Destroy()
    {
        instances.clear();
        buffer.reset();
        material.reset();
    }

    void Gizmos::Draw(GizmoQueue& instanceQueue, Mesh& mesh)
    {
        if (!instanceQueue.instances.empty())
        {
            instanceQueue.Flush();
            RenderingSystem.AddRendererInfo(RendererInfo{
                float4x4::Identity(), RenderQueue_Opaque, *instanceQueue.material, mesh,
                static_cast<uint32_t>(instanceQueue.instances.size())
            });
            instanceQueue.Clear();
        }
    }

    GizmosSystem& GizmosSystem::GetInstance()
    {
        static GizmosSystem instance = Engine::CreateSystem<GizmosSystem>();
        return instance;
    }

    void GizmosSystem::Update()
    {
        Gizmos::Draw(Gizmos::cuboidQueue, *Gizmos::cubeMesh);
        Gizmos::Draw(Gizmos::sphereQueue, *Gizmos::sphereMesh);
        Gizmos::Draw(Gizmos::wireCuboidQueue, *Gizmos::cubeMesh);
        Gizmos::Draw(Gizmos::wireSphereQueue, *Gizmos::sphereMesh);
    }
}