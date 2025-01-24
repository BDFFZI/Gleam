#include "LinesRendererSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightRendering/Runtime/Component/LinesRenderer.h"

namespace Light
{
    void LinesRendererSystem::Update()
    {
        View<LinesRenderer>::Each(Awake, [](LinesRenderer& linesRenderer)
        {
            linesRenderer.linesMesh = std::make_unique<Mesh>(true);
            linesRenderer.mesh = linesRenderer.linesMesh.get();
            if (linesRenderer.material == nullptr)
                linesRenderer.material = RenderingSystem->GetDefaultLineMaterial().get();
        });
        View<LinesRenderer>::Each([](LinesRenderer& linesRenderer)
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