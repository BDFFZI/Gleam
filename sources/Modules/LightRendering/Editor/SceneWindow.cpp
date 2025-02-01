#include "SceneWindow.h"

#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "LightRendering/Runtime/Component/Archetype.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Time.h"
#include <ImGuizmo.h>


namespace Light
{
    void ControlCamera(const class Input& input, LocalTransform& localTransform, LocalToWorld localToWorld)
    {
        if (input.GetMouseButton(MouseButton::Right) == false)
            return;

        const float deltaTime = Time->GetDeltaTime();
        const auto moveSpeed = static_cast<float>(4 * (input.GetKey(KeyCode::LeftShift) ? 3 : 1));

        float3 right = localToWorld.GetRight();
        float3 up = localToWorld.GetUp();
        float3 forward = localToWorld.GetForward();

        //位置调整
        float3 position = localTransform.position;
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
        localTransform.position = position;

        //视角调整
        Quaternion rotation = localTransform.rotation;
        const float2 mouseMoveDelta = input.GetMouseMoveDelta();
        rotation = Quaternion::AngleAxis(right, mouseMoveDelta.y * deltaTime * 50) * rotation;
        rotation = Quaternion::AngleAxis(up, mouseMoveDelta.x * deltaTime * 50) * rotation;
        localTransform.rotation = rotation;
    }

    void SceneWindow::Start()
    {
        preProcessSystem.onUpdate = [this]
        {
            //重建渲染目标和纹理
            if (isDirty && windowSize.x > 0 && windowSize.y > 0)
            {
                if (sceneCameraCanvasImID != nullptr)
                    UI::DeleteTexture(sceneCameraCanvasImID);
                sceneCameraCanvas = std::make_unique<GRenderTexture>(static_cast<int2>(windowSize));
                World::GetComponent<Camera>(sceneCamera).renderTarget = sceneCameraCanvas.get();
                sceneCameraCanvasImID = UI::CreateTexture(*sceneCameraCanvas);
                isDirty = false;
            }
            //相机控制
            ControlCamera(
                inputSystem,
                World::GetComponent<LocalTransform>(sceneCamera),
                World::GetComponent<LocalToWorld>(sceneCamera)
            );
        };
        World::AddSystem(&preProcessSystem);
        World::AddSystem(&inputSystem);

        sceneCamera = Awake->AddEntity(CameraArchetype);
    }
    void SceneWindow::Stop()
    {
        World::RemoveSystem(&preProcessSystem);
        World::RemoveSystem(&inputSystem);
        sceneCameraCanvas.reset();

        UI::DeleteTexture(sceneCameraCanvasImID);
    }
    void SceneWindow::Update()
    {
        ImGui::Begin("SceneWindow");
        //重建渲染纹理
        if (any(windowSize != UI::GetWindowContentRegionSize()))
        {
            windowSize = UI::GetWindowContentRegionSize();
            isDirty = true;
        }
        //绘制相机画面
        if (sceneCameraCanvasImID != nullptr)
            ImGui::Image(sceneCameraCanvasImID, windowSize);
        //绘制网格
        ImGuizmo::BeginFrame();
        const float2 windowPosition = ImGui::GetWindowContentRegionMin();
        ImGuizmo::SetRect(windowPosition.x, windowPosition.y, windowSize.x, windowSize.y);
        ImGuizmo::SetOrthographic(false);
        
        CameraTransform cameraTransform = World::GetComponent<CameraTransform>(sceneCamera);
        float4x4 objectToWorld = float4x4::Identity();
        ImGuizmo::DrawGrid(
            reinterpret_cast<float*>(&cameraTransform.worldToView),
            reinterpret_cast<float*>(&cameraTransform.viewToClip),
            reinterpret_cast<float*>(&objectToWorld),
            10
        );

        ImGui::End();
    }
}