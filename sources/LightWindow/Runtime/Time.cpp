#include "Time.h"

#include <GLFW/glfw3.h>

using namespace Light;

void Time::Start()
{
    timeReal = static_cast<float>(glfwGetTime());
}
void Time::Update()
{
    const float currentTimeReal = static_cast<float>(glfwGetTime());
    deltaTime = currentTimeReal - timeReal;
    timeReal = currentTimeReal;

    deltaTime *= timeScale;
    time += deltaTime;

    frameCount++;
}