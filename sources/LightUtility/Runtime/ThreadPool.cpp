#include "ThreadPool.h"

using namespace Light;

Worker::Worker()
    : thread([this](const std::stop_token& stopToken)
    {
        while (!stopToken.stop_requested())
        {
            executionSignal.acquire();
            task();
            if (taskFinished != nullptr)
                taskFinished();
            completionSignal.release();
        }
    })
{
}
Worker::~Worker()
{
    Execute([this]
    {
        thread.request_stop();
    });
}

void Worker::Execute(const std::function<void()>& task, const std::function<void()>& taskFinished)
{
    completionSignal.acquire();
    this->task = task;
    this->taskFinished = taskFinished;
    executionSignal.release();
}
bool Worker::IsCompleted()
{
    return completionSignal.try_acquire();
}

ThreadPool::~ThreadPool()
{
    WaitAll();
}
size_t ThreadPool::GetThreadCount()
{
    std::unique_lock lock(poolMutex);
    return workerPool.GetAllObjectsCount();
}

void ThreadPool::Schedule(const std::function<void()>& task, std::function<void()> taskFinished)
{
    poolMutex.lock();
    auto& worker = workerPool.Get();
    poolMutex.unlock();

    worker.Execute(task, [this,taskFinished = std::move(taskFinished),worker = &worker]
    {
        if (taskFinished != nullptr)taskFinished();
        std::unique_lock lock(poolMutex);
        workerPool.Release(*worker);
    });
}
void ThreadPool::WaitAll()
{
    while (true)
    {
        std::unique_lock lock(poolMutex);
        if (workerPool.GetUsableObjectsCount() == workerPool.GetAllObjectsCount())
            break;
    }
}
