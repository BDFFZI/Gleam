#include "PointsRendererSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightRendering/Runtime/Component/PointsRenderer.h"

namespace Light
{
    void PointsRendererSystem::Update()
    {
        View<PointsRenderer>::Each(Awake, [](PointsRenderer& pointsRenderer)
        {
            pointsRenderer.pointsMesh = std::make_unique<Mesh>(true);
            pointsRenderer.mesh = pointsRenderer.pointsMesh.get();
            if (pointsRenderer.material == nullptr)
                pointsRenderer.material = RenderingSystem->GetDefaultPointMaterial().get();
        });
        View<PointsRenderer>::Each([](PointsRenderer& pointsRenderer)
        {
            Mesh* pointMesh = pointsRenderer.pointsMesh.get();
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
    }
}