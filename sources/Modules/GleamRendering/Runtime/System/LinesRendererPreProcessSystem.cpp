#include "LinesRendererPreProcessSystem.h"

#include "GleamRendering/Runtime/Rendering.h"
#include "GleamRendering/Runtime/Entity/LinesMesh.h"

namespace Gleam
{
    void LinesRendererPreProcessSystem::Update()
    {
        World::GetView<LinesMesh>().Each([](LinesMesh& linesRenderer)
        {
            Mesh& lineMesh = *linesRenderer.linesMesh;
            std::vector<Vertex>& lineVertices = lineMesh.GetVertices();
            std::vector<uint32_t>& lineIndices = lineMesh.GetIndices();

            lineVertices.clear();
            lineIndices.clear();
            int lineIndex = 0;
            for (auto line : linesRenderer.lines)
            {
                lineVertices.emplace_back(line.pointA, 1.0f);
                lineIndices.emplace_back(lineIndex++);
                lineVertices.emplace_back(line.pointB, 1.0f);
                lineIndices.emplace_back(lineIndex++);
            }

            lineMesh.SetDirty();
        });
        World::GetView<LinesMesh, Renderer>().Each([](LinesMesh& linesRenderer, Renderer& renderer)
        {
            if (renderer.mesh.expired())
                renderer.mesh = linesRenderer.linesMesh;
            if (renderer.material.expired())
                renderer.material = Rendering::GetDefaultLineMaterial();
        });
    }
}