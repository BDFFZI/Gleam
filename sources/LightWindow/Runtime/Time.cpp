#include "Time.h"

#include <GLFW/glfw3.h>

using namespace Light;

void Time::Update()
{
    const float timeReal = GetTimeReal();
    deltaTime = timeReal - lastTimeReal;
    lastTimeReal = timeReal;

    deltaTime *= timeScale;
    time += deltaTime;

    frameCount++;
}

float Time::GetTimeReal()
{
    return static_cast<float>(glfwGetTime());
}
float Time::GetTime()
{
    return time;
}
float Time::GetDeltaTime()
{
    return deltaTime;
}
int Time::GetFrameCount()
{
    return frameCount;
}
