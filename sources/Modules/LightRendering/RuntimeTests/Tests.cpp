#include <gtest/gtest.h>

#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/System.h"
#include "LightEngine/Runtime/Component.h"
#include "LightRendering/Runtime/Component/Camera.h"

using namespace Light;

Light_MakeArchetype(CameraArchetype, Transform, Camera)
Light_MakeArchetypeMark(CameraAwakeArchetype, Transform, Camera, Awake)

// TEST(Rendering, Main)
void main()
{
    SystemEvent systemEvent{SimulationSystem};
    systemEvent.onStart = []
    {
        Camera camera;
        camera.background = float4{0.5, 0.5, 1, 1};
        World::AddEntity(CameraArchetype, camera);
    };
    World::AddSystem(systemEvent);

    Engine::Start();
}