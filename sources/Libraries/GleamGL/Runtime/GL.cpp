#include "GL.h"

#include <stdexcept>

GL GL::Initialize(GLFWwindow* window, const std::vector<const char*>& extensions, void* extensionFeatures)
{
    std::vector<const char*> validationLayers;
#ifdef _DEBUG
    validationLayers = {"VK_LAYER_KHRONOS_validation"};
    if (glInstance->CheckValidationLayerSupport(validationLayers) == false)
        throw std::runtime_error("不支持的验证层"); //启用验证层需要安装VulkanSDK
#else
    validationLayers = {};
#endif

    glfwWindow = window;
    glInstance = std::make_unique<GLInstance>(validationLayers);
    glSurface = std::make_unique<GLSurface>(*glInstance, window);
    glDevice = std::make_unique<GLDevice>(*glInstance, *glSurface, extensions, extensionFeatures);
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
