#pragma once
#include <GLFW/glfw3.h>

#include "LightEngine/Runtime/ECS/System.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    class Window : public SystemGroup
    {
    public:
        Window(): SystemGroup(nullptr, LeftOrder, MiddleOrder)
        {
        }

        int2 GetResolution() const;
        bool GetFullScreen() const;
        GLFWwindow* GetGlfwWindow() const { return glfwWindow; }
        void SetResolution(int width, int height) const;
        void SetFullScreen(bool fullscreen) const;

    private:
        GLFWwindow* glfwWindow = nullptr;

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Light_MakeSystem(Window)
}