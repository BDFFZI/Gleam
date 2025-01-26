#include "SceneWindow.h"

#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightMath/Runtime/MatrixMath.h"
#include "LightRendering/Runtime/Component/Archetype.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Time.h"


namespace Light
{
    void ControlCamera(const class Input& input, Transform& transform)
    {
        if (input.GetMouseButton(MouseButton::Right) == false)
            return;

        const float deltaTime = Time->GetDeltaTime();
        const auto moveSpeed = static_cast<float>(4 * (input.GetKey(KeyCode::LeftShift) ? 3 : 1));

        float3 right = transform.GetRight();
        float3 up = transform.GetUp();
        float3 forward = transform.GetForward();

        //位置调整
        float3 position = transform.GetLocalPosition();
        if (input.GetKey(KeyCode::W))
        {
            position += forward * deltaTime * moveSpeed;
        }
        if (input.GetKey(KeyCode::S))
        {
            position += -forward * deltaTime * moveSpeed;
        }
        if (input.GetKey(KeyCode::A))
        {
            position += -right * deltaTime * moveSpeed;
        }
        if (input.GetKey(KeyCode::D))
        {
            position += right * deltaTime * moveSpeed;
        }
        if (input.GetKey(KeyCode::Q))
        {
            position += -up * deltaTime * moveSpeed;
        }
        if (input.GetKey(KeyCode::E))
        {
            position += up * deltaTime * moveSpeed;
        }
        transform.SetLocalPosition(position);

        //视角调整
        float3 localEulerAngles = transform.GetLocalEulerAngles();
        const float2 mouseMoveDelta = input.GetMouseMoveDelta();
        localEulerAngles.y += mouseMoveDelta.x * deltaTime * 50;
        localEulerAngles.x += mouseMoveDelta.y * deltaTime * 50;
        transform.SetLocalEulerAngles(localEulerAngles);
    }

    void SceneWindow::Start()
    {
        World::AddSystem(&input);
        sceneCamera = Awake->AddEntity(CameraArchetype);
    }
    void SceneWindow::Stop()
    {
        World::RemoveSystem(&input);
        sceneCameraCanvas.reset();
        UI::DeleteTexture(sceneCameraCanvasImID);
    }
    void SceneWindow::Update()
    {
        if (all(lastWindowSize != UI::GetWindowContentRegionSize()))
        {
            lastWindowSize = UI::GetWindowContentRegionSize();
            sceneCameraCanvas = std::make_unique<GRenderTexture>(static_cast<int2>(lastWindowSize));
            World::GetComponent<Camera>(sceneCamera).renderTarget = sceneCameraCanvas.get();
            sceneCameraCanvasImID = UI::CreateTexture(*sceneCameraCanvas);
        }

        ImGui::Image(sceneCameraCanvasImID, lastWindowSize);

        ControlCamera(input, World::GetComponent<Transform>(sceneCamera));
    }
}