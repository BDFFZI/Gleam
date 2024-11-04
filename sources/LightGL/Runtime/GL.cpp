#include "GL.h"

GL GL::Initialize(GLFWwindow* window)
{
    std::vector<const char*> validationLayers;
#ifdef _DEBUG
    validationLayers = {"VK_LAYER_KHRONOS_validation"};
    if (glInstance->CheckValidationLayerSupport(validationLayers) == false)
        validationLayers.clear();
#else
    validationLayers = {};
#endif

    glfwWindow = window;
    glInstance = std::make_unique<GLInstance>(validationLayers);
    glSurface = std::make_unique<GLSurface>(*glInstance, window);
    glDevice = std::make_unique<GLDevice>(*glInstance, *glSurface);
    glCommandPool = std::make_unique<GLCommandPool>(*glDevice);

    return {};
}
void GL::UnInitialize()
{
    glCommandPool.reset();
    glDevice.reset();
    glSurface.reset();
    glInstance.reset();
}
