#include "LinesRendererSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamRendering/Runtime/Rendering.h"
#include "GleamRendering/Runtime/Component/LinesMesh.h"

namespace Gleam
{
    void LinesRendererSystem::Update()
    {
        View<LinesMesh>::Each([](LinesMesh& linesRenderer)
        {
            if (linesRenderer.linesMesh == std::nullopt)
            {
                if (linesRenderer.lines.empty() == false)
                    linesRenderer.linesMesh = std::make_unique<Mesh>(true);
                else
                    return;
            }

            std::unique_ptr<Mesh>& lineMesh = linesRenderer.linesMesh.value();
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
        View<LinesMesh, Renderer>::Each([](LinesMesh& linesRenderer, Renderer& renderer)
        {
            if (renderer.mesh == std::nullopt && linesRenderer.linesMesh.has_value())
                renderer.mesh = linesRenderer.linesMesh.value().get();
            if (renderer.material == std::nullopt)
                renderer.material = Rendering::GetDefaultLineMaterial().get();
        });
    }
}