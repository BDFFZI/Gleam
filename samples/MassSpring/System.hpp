#pragma once

#include <imgui.h>

#include "Component.hpp"
#include "LightECS/Runtime/System.hpp"
#include "LightECS/Runtime/View.hpp"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightGraphics/Runtime/Material.h"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightGraphics/Runtime/Shader.h"
#include "LightUI/Runtime/UI.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Time.h"
#include "LightWindow/Runtime/Window.h"

namespace Light
{
    struct BeginPaintSystem : System<MinSystemOrder, MaxSystemOrder>
    {
        inline static GLCommandBuffer* presentCommandBuffer = nullptr;
        inline static CommandBuffer* commandBuffer = nullptr;

        static void Update()
        {
            presentCommandBuffer = &Graphics::BeginPresent();
            commandBuffer = &Graphics::ApplyCommandBuffer();
            commandBuffer->BeginRecording();

            UI::BeginFrame();
        }
    };

    struct EndPaintSystem : System<BeginPaintSystem, MaxSystemOrder>
    {
        static void Update()
        {
            BeginPaintSystem::commandBuffer->BeginRendering(Graphics::GetPresentRenderTexture());
            UI::EndFrame(BeginPaintSystem::commandBuffer->GetGLCommandBuffer());
            BeginPaintSystem::commandBuffer->EndRendering();

            BeginPaintSystem::commandBuffer->EndRecording();
            BeginPaintSystem::presentCommandBuffer->ExecuteSubCommands(BeginPaintSystem::commandBuffer->GetGLCommandBuffer());
            Graphics::EndPresent(*BeginPaintSystem::presentCommandBuffer);

            Graphics::WaitPresent();
            Graphics::ReleaseCommandBuffer(*BeginPaintSystem::commandBuffer);
        }
    };

    struct PaintSystem : System<BeginPaintSystem, EndPaintSystem>
    {
        static void Start()
        {
            pointMesh = std::make_unique<MeshT<Vertex>>(&pointMeshLayout, true);
            lineMesh = std::make_unique<MeshT<Vertex>>(&lineMeshLayout, true);
            pointShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, pointMeshLayout);
            lineShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, lineMeshLayout);
            pointMaterial = std::make_unique<Material>(pointShader.get());
            lineMaterial = std::make_unique<Material>(lineShader.get());
        }
        static void Stop()
        {
            pointMaterial.reset();
            lineMaterial.reset();
            pointShader.reset();
            lineShader.reset();
            pointMesh.reset();
            lineMesh.reset();
        }

        static void DrawObject()
        {
            std::vector<Vertex>& pointVertices = pointMesh->GetVertices();
            std::vector<uint32_t>& pointIndices = pointMesh->GetIndices();
            pointVertices.clear();
            pointIndices.clear();
            int pointIndex = 0;
            View<Point, Renderer>::Each([&pointIndex,&pointVertices,&pointIndices](Point& point, Renderer& renderer)
            {
                pointVertices.emplace_back(point.position, renderer.color);
                pointIndices.emplace_back(pointIndex++);
            });

            std::vector<Vertex>& lineVertices = lineMesh->GetVertices();
            std::vector<uint32_t>& lineIndices = lineMesh->GetIndices();
            lineVertices.clear();
            lineIndices.clear();
            int lineIndex = 0;
            View<Line, Renderer>::Each([&lineIndex,&lineVertices,&lineIndices](Line& line, Renderer& renderer)
            {
                lineVertices.emplace_back(line.positionA, renderer.color);
                lineIndices.emplace_back(lineIndex++);
                lineVertices.emplace_back(line.positionB, renderer.color);
                lineIndices.emplace_back(lineIndex++);
            });


            auto& commandBuffer = *BeginPaintSystem::commandBuffer;
            commandBuffer.BeginRendering(Graphics::GetPresentRenderTexture(), true);
            commandBuffer.SetViewProjectionMatrices(float4x4::Identity(), float4x4::Ortho(50, 50, 0, 1));
            if (pointMesh->GetIndexCount() != 0)
                commandBuffer.Draw(pointMesh.get(), pointMaterial.get());
            if (lineMesh->GetIndexCount() != 0)
                commandBuffer.Draw(lineMesh.get(), lineMaterial.get());
            commandBuffer.EndRendering();
        }
        static void DrawUI()
        {
            ImGui::ShowDemoWindow();
        }
        static void Update()
        {
            DrawObject();
            DrawUI();
        }

    private:
        struct Vertex
        {
            float2 position;
            float4 color;
        };
        inline static GLVertexInput vertexInput = {
            sizeof(Vertex), {
                GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32_SFLOAT},
                GLVertexAttribute{1,offsetof(Vertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
            }
        };
        inline static GLMeshLayout pointMeshLayout = {
            vertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false}
        };
        inline static GLMeshLayout lineMeshLayout = {
            vertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false}
        };

        inline static std::unique_ptr<MeshT<Vertex>> pointMesh = nullptr;
        inline static std::unique_ptr<MeshT<Vertex>> lineMesh = nullptr;
        inline static std::unique_ptr<Shader> pointShader = nullptr;
        inline static std::unique_ptr<Shader> lineShader = nullptr;
        inline static std::unique_ptr<Material> pointMaterial = nullptr;
        inline static std::unique_ptr<Material> lineMaterial = nullptr;
    };

    struct LogicSystem : System<MinSystemOrder, PaintSystem>
    {
        static void Update()
        {
            if (Input::GetKeyDown(KeyCode::Esc))
            {
                Window::Stop();
            }
        }
    };

    struct PhysicsSystem : System<LogicSystem, PaintSystem>
    {
        constexpr static float DeltaTime = 0.02f;

        static void UpdateSpring()
        {
            View<Line, SpringPhysics>::Each([](Line& line, SpringPhysics& springPhysics)
            {
                Point pointA;
                MassPointPhysics massPointPhysicsA;
                World::GetComponents(springPhysics.pointA, &pointA, &massPointPhysicsA);
                Point pointB;
                MassPointPhysics massPointPhysicsB;
                World::GetComponents(springPhysics.pointB, &pointB, &massPointPhysicsB);

                line.positionA = pointA.position;
                line.positionB = pointB.position;
            });

            // View<Transform, RigidBody, SpringPhysics>::Each([](Transform& transform, RigidBody& rigidBody, SpringPhysics& spring)
            // {
            //     float vector = spring.pinPosition - transform.position;
            //     float direction = vector >= 0 ? 1 : -1;
            //     float distance = abs(vector) - spring.length;
            //     float elasticForce = spring.elasticity * distance * direction; //弹力或推力
            //     float resistance = -0.01f * spring.elasticity * (rigidBody.velocity * direction) * direction; //弹簧内部阻力（不添加无法使弹簧稳定）
            //     rigidBody.force += elasticForce + resistance;
            // });
        }
        static void UpdateMassPoint()
        {
            View<Point, MassPointPhysics>::Each([](Point& point, MassPointPhysics& massPointPhysics)
            {
                //牛顿第二定律
                float2 acceleration = massPointPhysics.force / massPointPhysics.mass;
                massPointPhysics.force = 0;
                //添加重力加速度
                if (Time::GetTime() > 5)
                    acceleration.y += -9.8f;
                //根据速度移动（速度不会自然衰减，牛顿第一定律）
                massPointPhysics.velocity += acceleration * DeltaTime;
                point.position += massPointPhysics.velocity * DeltaTime;
            });
        }

        static void Update()
        {
            float currentTime = Time::GetTime();
            float delta = currentTime - lastTime;
            while (delta >= DeltaTime)
            {
                UpdateMassPoint();
                UpdateSpring();
                delta -= DeltaTime;
                lastTime += DeltaTime;
            }
        }

    private:
        inline static float lastTime = 0;
    };
}
