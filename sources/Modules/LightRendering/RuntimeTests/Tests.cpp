#include <gtest/gtest.h>

#include "LightECS/Runtime/View.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/System.h"
#include "LightEngine/Runtime/Component.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/LinesRenderer.h"
#include "LightRendering/Runtime/Component/PointsRenderer.h"

using namespace Light;

Light_MakeArchetype(CameraArchetype, Transform, Camera)
Light_MakeArchetype(PointsArchetype, Transform, PointsRenderer)
Light_MakeArchetype(LinesArchetype, Transform, LinesRenderer)

// TEST(Rendering, Main)
void main()
{
    SystemEvent systemEvent{SimulationSystem};
    systemEvent.onStart = []
    {
        Camera camera;
        camera.background = float4{0.5, 0.5, 1, 1};
        Entity cameraEntity = Awake->AddEntity(CameraArchetype);
        World::SetComponents(cameraEntity, camera);


        float3 pointMap[5][5];
        std::vector<Point> points;
        constexpr int length = 4;
        for (int i = 0, y = -10; i < 5; i++, y += length)
            for (int j = 0, x = -10; j < 5; j++, x += length)
            {
                pointMap[i][j] = points.emplace_back(
                    float3({static_cast<float>(x), static_cast<float>(y)}, 1)
                ).position;
            }

        std::vector<Segment> lines;
        for (int y = 0; y < 5; y++)
            for (int x = 0; x < 5; x++)
            {
                if (x + 1 < 5)
                    lines.emplace_back(pointMap[y][x], pointMap[y][x + 1]);
                if (y + 1 < 5)
                    lines.emplace_back(pointMap[y][x], pointMap[y + 1][x]);
                if (x + 1 < 5 && y + 1 < 5)
                    lines.emplace_back(pointMap[y][x], pointMap[y + 1][x + 1]);
                if (x + 1 < 5 && y - 1 >= 0)
                    lines.emplace_back(pointMap[y][x], pointMap[y - 1][x + 1]);
                if (x + 2 < 5)
                    lines.emplace_back(pointMap[y][x], pointMap[y][x + 2]);
                if (y + 2 < 5)
                    lines.emplace_back(pointMap[y][x], pointMap[y + 2][x]);
            }

        Entity pointsRendererEntity = Awake->AddEntity(PointsArchetype);
        World::GetComponent<PointsRenderer>(pointsRendererEntity).points = points;
        Entity linesRendererEntity = Awake->AddEntity(LinesArchetype);
        World::GetComponent<LinesRenderer>(linesRendererEntity).lines = lines;
    };
    World::AddSystem(&systemEvent);

    Engine::Start();
}