#include "GLFoundation.h"

void GLFoundation::Initialize(GLFWwindow* window)
{
    std::vector validationLayers = {"VK_LAYER_KHRONOS_validation"};

    glInstance = std::make_unique<GLInstance>(validationLayers);
    glSurface = std::make_unique<GLSurface>(*glInstance, window);
    glDevice = std::make_unique<GLDevice>(*glInstance, *glSurface);
    glCommandPool = std::make_unique<GLCommandPool>(*glDevice);
}
