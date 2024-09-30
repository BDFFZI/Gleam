#include "ThreadPool.h"

using namespace LightRuntime;

ThreadPool::ThreadPool()
{
    thread = std::jthread([this]
    {
    });
}
