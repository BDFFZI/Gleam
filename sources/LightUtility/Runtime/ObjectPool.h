#pragma once
#include <functional>
#include <stack>
#include <memory>
#include <stdexcept>
#include <iostream>

namespace Light
{
    template <class TObject, auto... Args>
    class ObjectPool
    {
    public:
        ObjectPool() = default;
        ObjectPool(const ObjectPool&) = delete;
        ~ObjectPool()
        {
            if (GetUsableObjectsCount() != GetAllObjectsCount())
                throw std::runtime_error("对象池开始销毁，但物体并没有全部归还！");
            for (auto* object : managedObjectPool)
                delete object;
        }

        size_t GetUsableObjectsCount()
        {
            return usableObjectPool.size();
        }
        size_t GetAllObjectsCount()
        {
            return managedObjectPool.size();
        }

        TObject& Get()
        {
            TObject* object;
            if (usableObjectPool.empty())
            {
                object = new TObject(Args...);
                managedObjectPool.emplace_back(object);
            }
            else
            {
                object = usableObjectPool.top();
                usableObjectPool.pop();
            }

            return *object;
        }
        void Release(TObject& element)
        {
            usableObjectPool.push(&element);
        }

        ObjectPool& operator=(ObjectPool&&) = delete;

    private:
        std::stack<TObject*> usableObjectPool;
        std::vector<TObject*> managedObjectPool;
    };
}
