#pragma once

#include <thread>

#include "ObjectPool.h"

namespace LightRuntime
{
    class ThreadPool
    {
        class Worker : public std::thread
        {
            
        };

    public:
        ThreadPool();

    private:
        std::jthread thread;
    };
}
