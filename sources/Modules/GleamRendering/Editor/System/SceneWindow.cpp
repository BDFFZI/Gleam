#include "SceneWindow.h"

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Entity/Archetype.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMath/Runtime/Geometry/Geometry.h"
#include "GleamRendering/Editor/Handles.h"
#include "GleamWindow/Runtime/System/CursorSystem.h"

namespace Gleam
{
    void ControlCamera(
        class InputSystem& inputSystem, class TimeSystem& timeSystem,
        LocalTransform& localTransform, LocalToWorld localToWorld, Camera& camera,
        float& moveSpeed)
    {
        //用于旋转时保持欧拉角信息，以解决万向锁导致的旋转退化问题
        static float3 eulerAngles = 0;

        const float deltaTime = timeSystem.GetDeltaTime();
        const float moveDelta = deltaTime * static_cast<float>(4 * (inputSystem.GetKey(KeyCode::LeftShift) ? 3 : 1)) * moveSpeed;

        float3 right = localToWorld.GetRight();
        float3 up = localToWorld.GetUp();
        float3 forward = localToWorld.GetForward();

        if (inputSystem.GetMouseButtonDown(MouseButton::Right))
        {
            CursorSystem.SetLockState(true);
            CursorSystem.SetVisible(false);
            eulerAngles = localTransform.rotation.ToEulerAngles();
        }
        else if (inputSystem.GetMouseButtonUp(MouseButton::Right))
        {
            CursorSystem.SetLockState(false);
            CursorSystem.SetVisible(true);
        }
        else if (inputSystem.GetMouseButton(MouseButton::Right))
        {
            //速度调整
            moveSpeed = clamp(moveSpeed + inputSystem.GetMouseScrollDelta().y * deltaTime * 10, 0.01f, 3.0f);

            //位置调整
            float3 position = localTransform.position;
            if (inputSystem.GetKey(KeyCode::W))
            {
                position += forward * moveDelta;
            }
            if (inputSystem.GetKey(KeyCode::S))
            {
                position += -forward * moveDelta;
            }
            if (inputSystem.GetKey(KeyCode::A))
            {
                position += -right * moveDelta;
            }
            if (inputSystem.GetKey(KeyCode::D))
            {
                position += right * moveDelta;
            }
            if (inputSystem.GetKey(KeyCode::Q))
            {
                position += -up * moveDelta;
            }
            if (inputSystem.GetKey(KeyCode::E))
            {
                position += up * moveDelta;
            }
            localTransform.position = position;

            //视角调整
            const float2 mouseMoveDelta = inputSystem.GetMouseMoveDelta();
            eulerAngles.x += mouseMoveDelta.y * deltaTime * 50;
            eulerAngles.y += mouseMoveDelta.x * deltaTime * 50;
            localTransform.rotation = Quaternion::Euler(eulerAngles);
        }
        else
        {
            //鼠标滚轮视角缩放
            if (inputSystem.GetIsFocus())
            {
                if (camera.orthographic)
                    camera.halfHeight = max(1.0f, camera.halfHeight - inputSystem.GetMouseScrollDelta().y * moveDelta * 30);
                else
                    localTransform.position += forward * inputSystem.GetMouseScrollDelta().y * moveDelta * 30;
            }

            //鼠标中间平移
            if (inputSystem.GetMouseButton(MouseButton::Middle))
            {
                localTransform.position += right * -inputSystem.GetMouseMoveDelta().x * moveDelta;
                localTransform.position += up * inputSystem.GetMouseMoveDelta().y * moveDelta;
            }
        }
    }

    class SceneWindow& SceneWindow::GetSceneWindowDrawing()
    {
        return *sceneWindowDrawing;
    }
    const CustomUI& SceneWindow::GetCustomUI()
    {
        return sceneGUIs;
    }
    void SceneWindow::AddCustomUI(std::type_index typeIndex, const std::function<void(void*)>& drawSceneUI)
    {
        sceneGUIs.insert({typeIndex, drawSceneUI});
    }
    Entity SceneWindow::GetSceneCamera() const
    {
        return sceneCamera;
    }
    InputSystem_T& SceneWindow::GetSceneInputSystem()
    {
        return inputSystem;
    }
    TimeSystem_T& SceneWindow::GetSceneTimeSystem()
    {
        return timeSystem;
    }
    int SceneWindow::GetHandleOption() const
    {
        return handleOption;
    }


    void SceneWindow::Start()
    {
        windowContentSize = 0; //以便重启时能触发纹理重建
        preProcessSystem.OnUpdate() = [this]
        {
            //重建渲染目标和纹理
            if (isDirty && windowContentSize.x > 0 && windowContentSize.y > 0)
            {
                isDirty = false;
                if (sceneCameraCanvasImID != nullptr)
                    UI::DeleteTexture(sceneCameraCanvasImID);
                sceneCameraCanvas = std::make_unique<GRenderTexture>(static_cast<int2>(windowContentSize));
                World::GetComponent<Camera>(sceneCamera).renderTarget = *sceneCameraCanvas;
                sceneCameraCanvasImID = UI::CreateTexture(*sceneCameraCanvas);
            }
            //相机控制
            Camera& camera = World::GetComponent<Camera>(sceneCamera);
            LocalTransform& cameraTransform = World::GetComponent<LocalTransform>(sceneCamera);
            LocalToWorld& cameraLocalToWorld = World::GetComponent<LocalToWorld>(sceneCamera);
            ControlCamera(inputSystem, timeSystem, cameraTransform, cameraLocalToWorld, camera, moveSpeed);
            cameraTransformSaving = cameraTransform;
            cameraSaving = camera;
            cameraSaving.renderTarget = std::nullopt;
        };
        World::AddSystem(preProcessSystem);
        World::AddSystem(inputSystem);
        World::AddSystem(timeSystem);

        sceneCamera = World::AddEntity(SceneCameraArchetype);
        World::SetComponents(sceneCamera, cameraTransformSaving);
        World::SetComponents(sceneCamera, cameraSaving);
    }
    void SceneWindow::Stop()
    {
        World::RemoveSystem(preProcessSystem);
        World::RemoveSystem(inputSystem);
        World::RemoveSystem(timeSystem);
        sceneCameraCanvas.reset();

        UI::DeleteTexture(sceneCameraCanvasImID);
    }
    void SceneWindow::Update()
    {
        sceneWindowDrawing = this;

        ImGui::Begin("SceneWindow", nullptr, ImGuiWindowFlags_MenuBar);
        //渲染纹理重建检查（别在获取窗口信息后执行！）
        if (any(windowContentSize != UI::GetWindowContentRegionSize()))
            isDirty = true;
        //获取窗口信息
        windowContentPosition = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
        windowContentSize = UI::GetWindowContentRegionSize();
        //设置输入区域范围
        inputSystem.SetFocusArea(Rectangle::CreateFromOrigin(windowContentPosition, windowContentSize));

        //绘制菜单选项
        if (ImGui::BeginMenuBar())
        {
            ImGui::PushItemWidth(windowContentSize.x / 5);
            //相机属性
            if (ImGui::Button("Camera"))
                InspectorWindow::Show(sceneCamera);
            ImGui::Checkbox("Ortho", &World::GetComponent<Camera>(sceneCamera).orthographic);
            ImGui::DragFloat("Speed", &moveSpeed);
            // 显示SceneUI（如手柄，网格等）
            ImGui::Checkbox("SceneUI", &showSceneUI);

            ImGui::PopItemWidth();

            // //手柄选项
            // {
            //     ImGui::SetNextItemWidth(windowContentSize.x * 0.5f);
            //     static const char* optionName[] = {"Hide", "Position", "Rotation", "Scale"};
            //     ImGui::Combo("Handle", &handleOption, optionName, std::size(optionName));
            //     if (inputSystem.GetMouseButton(MouseButton::Right) == false)
            //     {
            //         if (ImGui::IsKeyPressed(ImGuiKey_Q))handleOption = 0;
            //         if (ImGui::IsKeyPressed(ImGuiKey_W))handleOption = 1;
            //         if (ImGui::IsKeyPressed(ImGuiKey_E))handleOption = 2;
            //         if (ImGui::IsKeyPressed(ImGuiKey_R))handleOption = 3;
            //     }
            // }


            ImGui::EndMenuBar();
        }


        //绘制相机画面
        if (sceneCameraCanvasImID != nullptr)
            ImGui::Image(sceneCameraCanvasImID, windowContentSize);

        //绘制Gizmos
        if (showSceneUI)
        {
            Camera camera = World::GetComponent<Camera>(sceneCamera);
            WorldToLocal cameraWorldToLocal = World::GetComponent<WorldToLocal>(sceneCamera);
            ViewToClip cameraViewToClip = World::GetComponent<ViewToClip>(sceneCamera);

            ImGuizmo::SetDrawlist(); //使Gizmos能绘制到场景画面前面
            ImGuizmo::SetOrthographic(camera.orthographic);
            ImGuizmo::SetRect(windowContentPosition.x, windowContentPosition.y, windowContentSize.x, windowContentSize.y); //设置绘制区域

            //绘制网格线
            {
                float4x4 objectToWorld = float4x4::Identity();
                ImGuizmo::DrawGrid(
                    reinterpret_cast<float*>(&cameraWorldToLocal.value),
                    reinterpret_cast<float*>(&cameraViewToClip.value),
                    reinterpret_cast<float*>(&objectToWorld), 10
                );
            }
            //绘制右上角世界轴
            float size = min(windowContentSize.x, windowContentSize.y) * 0.2f;
            Rectangle rectangle = Rectangle::CreateFromOrigin({windowContentSize.x - size, 0}, {size});
            ImGuizmo::ViewManipulate(
                cameraWorldToLocal.value.data, 1,
                windowContentPosition + rectangle.min + rectangle.GetSize(), -rectangle.GetSize(), 0x10FFFFFF);
            if (Geometry::Contains(rectangle, inputSystem.GetMousePosition()))
            {
                float3 position;
                float3x3 rotation;
                float3 scale;
                DecomposeTRS(inverse(cameraWorldToLocal.value), position, rotation, scale);
                World::GetComponent<LocalTransform>(sceneCamera).rotation = Quaternion::Matrix(rotation);
            }

            //绘制自定义UI或Gizmos
            void* target = InspectorWindow.GetTarget();
            if (target != nullptr)
            {
                ImGui::SetCursorPos({});
                Handles::WorldToView() = cameraWorldToLocal.value;
                Handles::ViewToClip() = cameraViewToClip.value;
                if (sceneGUIs.contains(InspectorWindow.GetTargetType()))
                    sceneGUIs[InspectorWindow.GetTargetType()](target);
            }
        }

        ImGui::End();
    }
}