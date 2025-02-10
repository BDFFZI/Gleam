#include "PointsRendererSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamRendering/Runtime/Rendering.h"
#include "GleamRendering/Runtime/Component/PointsMesh.h"

namespace Gleam
{
    void PointsRendererSystem::Update()
    {
        View<PointsMesh>::Each([](PointsMesh& pointsRenderer)
        {
            if (pointsRenderer.pointsMesh == std::nullopt)
            {
                if (pointsRenderer.points.empty() == false)
                    pointsRenderer.pointsMesh = std::make_unique<Mesh>(true);
                else
                    return;
            }

            std::unique_ptr<Mesh>& pointMesh = pointsRenderer.pointsMesh.value();
            std::vector<Vertex>& pointVertices = pointMesh->GetVertices();
            std::vector<uint32_t>& pointIndices = pointMesh->GetIndices();

            pointVertices.clear();
            pointIndices.clear();
            int pointIndex = 0;
            for (auto point : pointsRenderer.points)
            {
                pointVertices.emplace_back(point.position, 1.0f);
                pointIndices.emplace_back(pointIndex++);
            }

            pointMesh->SetDirty();
        });
        View<PointsMesh, Renderer>::Each([](PointsMesh& pointsRenderer, Renderer& renderer)
        {
            if (renderer.mesh == std::nullopt && pointsRenderer.pointsMesh.has_value())
                renderer.mesh = pointsRenderer.pointsMesh.value().get();
            if (renderer.material == std::nullopt)
                renderer.material = Rendering::GetDefaultPointMaterial().get();
        });
    }
}