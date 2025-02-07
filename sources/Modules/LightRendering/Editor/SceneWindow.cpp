#include "SceneWindow.h"

#include "LightECS/Runtime/World.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "LightRendering/Runtime/Entity/Archetype.h"
#include "LightWindow/Runtime/Input.h"
#include <ImGuizmo.h>

#include "LightEngine/Editor/InspectorWindow.h"
#include "LightEngine/Runtime/System/TimeSystem.h"
#include "LightWindow/Runtime/Cursor.h"

namespace Light
{
    float3 eulerAngles;
    void ControlCamera(const class Input& input, LocalTransform& localTransform, LocalToWorld localToWorld)
    {
        const float deltaTime = TimeSystem->GetDeltaTime();
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
        const float2 mouseMoveDelta = input.GetMouseMoveDelta();
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
            if (inputSystem.GetMouseButtonDown(MouseButton::Right))
            {
                Cursor->SetLockState(true);
                Cursor->SetVisible(false);
            }
            else if (inputSystem.GetMouseButtonUp(MouseButton::Right))
            {
                Cursor->SetLockState(false);
                Cursor->SetVisible(true);
            }
            else if (inputSystem.GetMouseButton(MouseButton::Right))
            {
                ControlCamera(
                    inputSystem,
                    World::GetComponent<LocalTransform>(sceneCamera),
                    World::GetComponent<LocalToWorld>(sceneCamera)
                );
            }
        };
        World::AddSystem(&preProcessSystem);
        World::AddSystem(&inputSystem);

        sceneCamera = World::AddEntity(CameraArchetype);

        ImGuizmo::AllowAxisFlip(false); //禁用手柄轴自动反转
        ImGuizmo::SetGizmoSizeClipSpace(0.2f); //设置手柄在剪辑空间的大小
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
        ImGui::Begin("SceneWindow", nullptr, ImGuiWindowFlags_MenuBar);
        //获取窗口信息
        windowPosition = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
        windowSize = UI::GetWindowContentRegionSize();
        //设置输入区域范围
        inputSystem.SetFocusArea({windowPosition, windowSize});
        //渲染纹理重建检查
        if (any(windowSize != UI::GetWindowContentRegionSize()))
            isDirty = true;

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