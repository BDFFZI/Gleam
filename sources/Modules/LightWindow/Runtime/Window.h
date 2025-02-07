#pragma once
#include <GLFW/glfw3.h>

#include "InputEnum.h"
#include "LightECS/Runtime/System.h"
#include "LightEngine/Runtime/System/UpdateSystem.h"
#include "LightMath/Runtime/LinearAlgebra/Vector.h"

namespace Light
{
    class Window : public SystemGroup
    {
    public:
        Window(): SystemGroup(PreUpdateSystem, LeftOrder, MiddleOrder)
        {
        }

        GLFWwindow* GetGlfwWindow() const { return glfwWindow; }
        int2 GetWindowPosition() const;
        int2 GetResolution() const;
        bool GetFullScreen() const;

        float2 GetMousePosition() const { return mousePosition; }
        float2 GetMouseScrollDelta() const { return mouseScrollDelta; }
        bool GetMouseButtonState(const MouseButton button) const { return mouseButtonStates[static_cast<uint8_t>(button)]; }
        bool GetKeyboardState(const KeyCode key) const { return keyboardStates[static_cast<uint16_t>(key)]; }
        bool IsMouseDown() const { return isMouseDown; }

        void SetResolution(int2 resolution) const;
        void SetFullScreen(bool fullscreen) const;
        void SetMousePosition(float2 position) const;

    private:
        GLFWwindow* glfwWindow = nullptr;
        float2 mousePosition = {};
        float2 mouseScrollDelta = {};
        bool mouseButtonStates[3] = {};
        bool keyboardStates[349] = {};
        bool lastMouseState = false;
        bool isMouseDown = false;

        void Start() override;
        void Stop() override;
        void Update() override;

        friend void GlfwCursorPosCallback(GLFWwindow* window, double xPos, double yPos);
        friend void GlfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
        friend void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        friend void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
    Light_MakeGlobalSystem(Window)

    void GlfwWindowFocusCallback(GLFWwindow* window, int focused);
    void GlfwCursorEnterCallback(GLFWwindow* window, int entered);
    void GlfwCursorPosCallback(GLFWwindow* window, double xPos, double yPos);
    void GlfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void GlfwCharCallback(GLFWwindow* window, unsigned int codepoint);
}