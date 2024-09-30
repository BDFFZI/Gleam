#pragma once

#include <queue>
#include <thread>
#include <condition_variable>

#include "ObjectPool.h"

namespace LightRuntime
{
    class ThreadPool
    {
    public:
        void Submit(const std::function<void()>& task);

    private:
        // struct Worker
        // {
        //     std::mutex mutex;
        //     std::condition_variable conditionVariable;
        //     std::function<void()> task;
        //     std::jthread thread = std::jthread([this]
        //     {
        //         std::unique_lock lock(mutex);
        //
        //         while (true)
        //         {
        //         }
        //
        //
        //         conditionVariable.wait()
        //         while (std::thread::)
        //         {
        //         }
        //     });
        // };
        //
        // std::queue<std::function<void()>> tasks;

        // void ThreadUpdate();
        //
        // ObjectPool<std::jthread,&ThreadUpdate> pool;
    };
}
