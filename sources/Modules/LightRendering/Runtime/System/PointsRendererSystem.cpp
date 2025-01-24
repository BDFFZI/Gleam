#include "PointsRendererSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightRendering/Runtime/Component/PointsMesh.h"

namespace Light
{
    void PointsRendererSystem::Update()
    {
        View<PointsMesh>::Each(Awake, [](PointsMesh& pointsRenderer)
        {
            pointsRenderer.pointsMesh = std::make_unique<Mesh>(true);
        });
        View<PointsMesh, Renderer>::Each(Awake, [](PointsMesh& pointsRenderer, Renderer& renderer)
        {
            renderer.mesh = pointsRenderer.pointsMesh.get();
            if (renderer.material == nullptr)
                renderer.material = RenderingSystem->GetDefaultPointMaterial().get();
        });
        View<PointsMesh>::Each([](PointsMesh& pointsRenderer)
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