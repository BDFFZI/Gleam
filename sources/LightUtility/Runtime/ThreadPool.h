#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <semaphore>

#include "ObjectPool.h"

namespace Light
{
    class Worker
    {
    public:
        Worker();
        ~Worker();

        void Execute(const std::function<void()>& task, const std::function<void()>& taskFinished = nullptr);
        bool IsCompleted();

    private:
        std::binary_semaphore executionSignal{0};
        std::binary_semaphore completionSignal{1};
        std::function<void()> task;
        std::function<void()> taskFinished;
        std::jthread thread;
    };

    class ThreadPool
    {
    public:
        ~ThreadPool();
        size_t GetThreadCount();

        void Schedule(const std::function<void()>& task, std::function<void()> taskFinished = nullptr);
        void WaitAll();

    private:
        std::mutex poolMutex;
        ObjectPool<Worker> workerPool;
    };
}
