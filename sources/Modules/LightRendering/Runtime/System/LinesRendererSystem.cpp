#include "LinesRendererSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightRendering/Runtime/Component/LinesMesh.h"

namespace Light
{
    void LinesRendererSystem::Update()
    {
        View<LinesMesh>::Each(Awake, [](LinesMesh& linesRenderer)
        {
            linesRenderer.linesMesh = std::make_unique<Mesh>(true);
        });
        View<LinesMesh, Renderer>::Each(Awake, [](LinesMesh& linesRenderer, Renderer& renderer)
        {
            linesRenderer.linesMesh = std::make_unique<Mesh>(true);
            renderer.mesh = linesRenderer.linesMesh.get();
            if (renderer.material == nullptr)
                renderer.material = RenderingSystem->GetDefaultLineMaterial().get();
        });
        View<LinesMesh>::Each([](LinesMesh& linesRenderer)
        {
            Mesh* lineMesh = linesRenderer.linesMesh.get();
            std::vector<Vertex>& lineVertices = lineMesh->GetVertices();
            std::vector<uint32_t>& lineIndices = lineMesh->GetIndices();

            lineVertices.clear();
            lineIndices.clear();
            int lineIndex = 0;
            for (auto line : linesRenderer.lines)
            {
                lineVertices.emplace_back(line.positionA, 1.0f);
                lineIndices.emplace_back(lineIndex++);
                lineVertices.emplace_back(line.positionB, 1.0f);
                lineIndices.emplace_back(lineIndex++);
            }

            lineMesh->SetDirty();
        });
    }
}