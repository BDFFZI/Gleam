#include "Gizmos.h"

#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Rendering.h"

struct InstanceInfo;

namespace Gleam
{
    void Gizmos::Init()
    {
        //着色器代码布局
        GSCodeLayout codeLayout = GSCodeLayout{"Resources/GleamRenderingEditor/Gizmo.hlsl"};
        //着色器资源布局
        std::vector<GLDescriptorBinding> descriptorBindings = RenderingConfig::DescriptorBindings;
        descriptorBindings.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        assetLayout = std::make_unique<GSAssetLayout>(descriptorBindings, RenderingConfig::PushConstantRanges);
        //线框着色状态布局
        GSStateLayout stateLayout = Graphics::GetGraphicsConfig().defaultGSStateLayout;
        stateLayout.rasterization.polygonMode = VK_POLYGON_MODE_LINE;
        //创建着色器
        gizmoShader = std::make_unique<GShader>(codeLayout, Graphics::GetGraphicsConfig().defaultGSStateLayout, *assetLayout);
        wireGizmoShader = std::make_unique<GShader>(codeLayout, stateLayout, *assetLayout);

        //创建点线材质
        pointsMaterial = std::make_unique<Material>(Rendering::GetDefaultPointShader());
        linesMaterial = std::make_unique<Material>(Rendering::GetDefaultLineShader());

        //创建原型网格
        cubeMesh = std::make_unique<Mesh>(ModelImporter::ImportObj("Resources/GleamRenderingEditor/Cube.obj"));
        sphereMesh = std::make_unique<Mesh>(ModelImporter::ImportObj("Resources/GleamRenderingEditor/Sphere.obj"));

        //创建渲染数据容器
        cuboidQueue = {std::make_unique<Material>(*gizmoShader, *assetLayout)};
        sphereQueue = {std::make_unique<Material>(*gizmoShader, *assetLayout)};
        wireCuboidQueue = {std::make_unique<Material>(*wireGizmoShader, *assetLayout)};
        wireSphereQueue = {std::make_unique<Material>(*wireGizmoShader, *assetLayout)};
        pointsMesh = std::make_unique<Mesh>(true);
        linesMesh = std::make_unique<Mesh>(true);
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
        pointsMesh.reset();
        pointsMaterial.reset();
        linesMesh.reset();
        linesMaterial.reset();
    }

    void Gizmos::PushLocalToWorld(const float4x4& localToWorld)
    {
        localToWorlds.push_back(localToWorld);
    }
    void Gizmos::PopLocalToWorld()
    {
        localToWorlds.pop_back();
    }
    void Gizmos::Draw(const Point& point, const float4& color)
    {
        pointsMesh->GetVertices().emplace_back(Vertex{
            mul(localToWorlds.back(), float4(point.position, 1)).xyz, color
        }); // NOLINT(clang-diagnostic-missing-field-initializers)
        pointsMesh->GetIndices().emplace_back(static_cast<uint32_t>(pointsMesh->GetIndices().size()));
    }
    void Gizmos::Draw(const float3& point, const float4& color)
    {
        Draw(Point{point}, color);
    }
    void Gizmos::Draw(const Segment& segment, const float4& color)
    {
        linesMesh->GetVertices().insert(
            linesMesh->GetVertices().end(), {
                Vertex{mul(localToWorlds.back(), float4(segment.pointA, 1)).xyz, color}, // NOLINT(clang-diagnostic-missing-field-initializers)
                Vertex{mul(localToWorlds.back(), float4(segment.pointB, 1)).xyz, color} // NOLINT(clang-diagnostic-missing-field-initializers)
            });
        linesMesh->GetIndices().emplace_back(static_cast<uint32_t>(linesMesh->GetIndices().size()));
        linesMesh->GetIndices().emplace_back(static_cast<uint32_t>(linesMesh->GetIndices().size()));
    }

    void Gizmos::Draw(const Cuboid& cuboid, const float4& color)
    {
        cuboidQueue.instances.emplace_back(
            mul(localToWorlds.back(), float4x4::TRS(cuboid.GetCenter(), float3{0}, cuboid.GetSize())),
            color
        );
    }
    void Gizmos::Draw(const Rectangle& rectangle, const float4& color)
    {
        Draw(Cuboid{float3{rectangle.min, 0}, float3{rectangle.max, 0}}, color);
    }
    void Gizmos::Draw(const Sphere& sphere, const float4& color)
    {
        sphereQueue.instances.emplace_back(
            mul(localToWorlds.back(), float4x4::TRS(sphere.center, float3{0}, sphere.radius * 2)),
            color
        );
    }
    void Gizmos::DrawWire(const Cuboid& cuboid, const float4& color)
    {
        wireCuboidQueue.instances.emplace_back(
            mul(localToWorlds.back(), float4x4::TRS(cuboid.GetCenter(), float3{0}, cuboid.GetSize())),
            color
        );
    }
    void Gizmos::DrawWire(const Rectangle& rectangle, const float4& color)
    {
        DrawWire(Cuboid{float3{rectangle.min, 0}, float3{rectangle.max, 0}}, color);
    }
    void Gizmos::DrawWire(const Sphere& sphere, const float4& color)
    {
        wireSphereQueue.instances.emplace_back(
            mul(localToWorlds.back(), float4x4::TRS(sphere.center, float3{0}, sphere.radius * 2)),
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
            GlobalRenderingSystem.AddRendererInfo(RendererInfo{
                float4x4::Identity(), RenderQueue_Opaque, *instanceQueue.material, mesh,
                static_cast<uint32_t>(instanceQueue.instances.size())
            });
            instanceQueue.Clear();
        }
    }
}