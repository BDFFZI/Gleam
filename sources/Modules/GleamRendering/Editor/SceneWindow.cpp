#include "SceneWindow.h"

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamWindow/Runtime/System/InputSystem.h"
#include <ImGuizmo.h>

#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamWindow/Runtime/System/CursorSystem.h"

namespace Gleam
{
    float3 eulerAngles;
    void ControlCamera(
        class InputSystem& inputSystem, class TimeSystem& timeSystem,
        LocalTransform& localTransform, LocalToWorld localToWorld)
    {
        const float deltaTime = timeSystem.GetDeltaTime();
        const auto moveSpeed = static_cast<float>(4 * (inputSystem.GetKey(KeyCode::LeftShift) ? 3 : 1));

        float3 right = localToWorld.GetRight();
        float3 up = localToWorld.GetUp();
        float3 forward = localToWorld.GetForward();

        //位置调整
        float3 position = localTransform.position;
        if (inputSystem.GetKey(KeyCode::W))
        {
            position += forward * deltaTime * moveSpeed;
        }
        if (inputSystem.GetKey(KeyCode::S))
        {
            position += -forward * deltaTime * moveSpeed;
        }
        if (inputSystem.GetKey(KeyCode::A))
        {
            position += -right * deltaTime * moveSpeed;
        }
        if (inputSystem.GetKey(KeyCode::D))
        {
            position += right * deltaTime * moveSpeed;
        }
        if (inputSystem.GetKey(KeyCode::Q))
        {
            position += -up * deltaTime * moveSpeed;
        }
        if (inputSystem.GetKey(KeyCode::E))
        {
            position += up * deltaTime * moveSpeed;
        }
        localTransform.position = position;

        //视角调整
        const float2 mouseMoveDelta = inputSystem.GetMouseMoveDelta();
        eulerAngles.x += mouseMoveDelta.y * deltaTime * 50;
        eulerAngles.y += mouseMoveDelta.x * deltaTime * 50;
        localTransform.rotation = Quaternion::Euler(eulerAngles);
    }

    const CustomGUI& SceneWindow::GetCustomGUI()
    {
        return sceneGUIs;
    }
    void SceneWindow::AddCustomGUI(std::type_index typeIndex, const std::function<void(void*)>& drawSceneGUI)
    {
        sceneGUIs.insert({typeIndex, drawSceneGUI});
    }
    const Camera& SceneWindow::GetCamera() const
    {
        return camera;
    }
    const WorldToClip& SceneWindow::GetCameraTransform() const
    {
        return cameraTransform;
    }
    int SceneWindow::GetHandleOption() const
    {
        return handleOption;
    }
    void SceneWindow::Start()
    {
        windowSize = 0; //以便重启时能触发纹理重建
        preProcessSystem.onUpdate = [this]
        {
            //重建渲染目标和纹理
            if (isDirty && windowSize.x > 0 && windowSize.y > 0)
            {
                isDirty = false;
                if (sceneCameraCanvasImID != nullptr)
                    UI::DeleteTexture(sceneCameraCanvasImID);
                sceneCameraCanvas = std::make_unique<GRenderTexture>(static_cast<int2>(windowSize));
                World::GetComponent<Camera>(sceneCamera).renderTarget = sceneCameraCanvas.get();
                sceneCameraCanvasImID = UI::CreateTexture(*sceneCameraCanvas);
            }
            //相机控制
            if (inputSystem.GetMouseButtonDown(MouseButton::Right))
            {
                CursorSystem->SetLockState(true);
                CursorSystem->SetVisible(false);
            }
            else if (inputSystem.GetMouseButtonUp(MouseButton::Right))
            {
                CursorSystem->SetLockState(false);
                CursorSystem->SetVisible(true);
            }
            else if (inputSystem.GetMouseButton(MouseButton::Right))
            {
                ControlCamera(
                    inputSystem, timeSystem,
                    World::GetComponent<LocalTransform>(sceneCamera),
                    World::GetComponent<LocalToWorld>(sceneCamera)
                );
                cameraLocalTransform = World::GetComponent<LocalTransform>(sceneCamera);
            }
        };
        World::AddSystem(&preProcessSystem);
        World::AddSystem(&inputSystem);
        World::AddSystem(&timeSystem);

        sceneCamera = World::AddEntity(CameraArchetype);
        World::SetComponents(sceneCamera, cameraLocalTransform);

        ImGuizmo::AllowAxisFlip(false); //禁用手柄轴自动反转
        ImGuizmo::SetGizmoSizeClipSpace(0.2f); //设置手柄在剪辑空间的大小
    }
    void SceneWindow::Stop()
    {
        World::RemoveSystem(&preProcessSystem);
        World::RemoveSystem(&inputSystem);
        World::RemoveSystem(&timeSystem);
        sceneCameraCanvas.reset();

        UI::DeleteTexture(sceneCameraCanvasImID);
    }
    void SceneWindow::Update()
    {
        ImGui::Begin("SceneWindow", nullptr, ImGuiWindowFlags_MenuBar);
        //渲染纹理重建检查（别在获取窗口信息后执行！）
        if (any(windowSize != UI::GetWindowContentRegionSize()))
            isDirty = true;
        //获取窗口信息
        windowPosition = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
        windowSize = UI::GetWindowContentRegionSize();
        //设置输入区域范围
        inputSystem.SetFocusArea({windowPosition, windowSize});

        //绘制菜单选项
        if (ImGui::BeginMenuBar())
        {
            // 查看场景相机
            if (ImGui::MenuItem("EditorCamera"))
            {
                InspectorWindow::Show(sceneCamera);
            }
            //手柄选项
            {
                ImGui::SetNextItemWidth(windowSize.x * 0.5f);
                static const char* optionName[] = {"Hide", "Position", "Rotation", "Scale"};
                ImGui::Combo("Handle", &handleOption, optionName, std::size(optionName));
                if (inputSystem.GetMouseButton(MouseButton::Right) == false)
                {
                    if (ImGui::IsKeyPressed(ImGuiKey_Q))handleOption = 0;
                    if (ImGui::IsKeyPressed(ImGuiKey_W))handleOption = 1;
                    if (ImGui::IsKeyPressed(ImGuiKey_E))handleOption = 2;
                    if (ImGui::IsKeyPressed(ImGuiKey_R))handleOption = 3;
                }
            }

            ImGui::EndMenuBar();
        }

        //绘制相机画面
        if (sceneCameraCanvasImID != nullptr)
            ImGui::Image(sceneCameraCanvasImID, windowSize);

        //绘制Gizmos
        {
            ImGuizmo::BeginFrame();
            ImGuizmo::SetDrawlist(); //使Gizmos能绘制到场景画面前面
            //设置绘制区域
            const float2 windowPosition = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
            ImGuizmo::SetRect(windowPosition.x, windowPosition.y, windowSize.x, windowSize.y);
            //获取并设置相机属性
            camera = World::GetComponent<Camera>(sceneCamera);
            cameraTransform = World::GetComponent<WorldToClip>(sceneCamera);
            ImGuizmo::SetOrthographic(camera.orthographic);
            //绘制网格线
            {
                float4x4 objectToWorld = float4x4::Identity();
                ImGuizmo::DrawGrid(
                    reinterpret_cast<float*>(&cameraTransform.worldToView),
                    reinterpret_cast<float*>(&cameraTransform.viewToClip),
                    reinterpret_cast<float*>(&objectToWorld), 10
                );
            }
        }

        //绘制自定义UI和Gizmos
        void* target = InspectorWindow->GetTarget();
        if (target != nullptr)
        {
            ImGui::SetCursorPos({});
            if (sceneGUIs.contains(InspectorWindow->GetTargetType()))
                sceneGUIs[InspectorWindow->GetTargetType()](target);
        }

        ImGui::End();
    }
}