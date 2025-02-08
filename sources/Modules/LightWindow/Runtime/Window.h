#pragma once
#include <GLFW/glfw3.h>

#include "InputEnum.h"
#include "LightMath/Runtime/LinearAlgebra/Vector.h"

namespace Light
{
    class Window
    {
    public:
        static GLFWwindow* GetGlfwWindow();

        static int2 GetWindowPosition();
        static int2 GetResolution();
        static bool GetFullScreen();
        static float2 GetMousePosition();
        static float2 GetMouseScrollDelta();
        static bool GetIsMouseDown();
        static bool GetMouseButtonState(MouseButton button);
        static bool GetKeyboardState(KeyCode key);
        static void SetResolution(int2 resolution);
        static void SetFullScreen(bool fullscreen);
        static void SetMousePosition(float2 position);

        static void Init();
        static void UnInit();
        static void Update();

    private:
        inline static GLFWwindow* glfwWindow = nullptr;
        inline static float2 mousePosition = {};
        inline static float2 mouseScrollDelta = {};
        inline static bool mouseButtonStates[3] = {};
        inline static bool keyboardStates[349] = {};
        inline static bool lastMouseState = false;
        inline static bool isMouseDown = false;

        static void GlfwWindowFocusCallback(GLFWwindow* window, int focused);
        static void GlfwCursorEnterCallback(GLFWwindow* window, int entered);
        static void GlfwCursorPosCallback(GLFWwindow* window, double xPos, double yPos);
        static void GlfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
        static void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void GlfwCharCallback(GLFWwindow* window, unsigned int codepoint);
    };
}