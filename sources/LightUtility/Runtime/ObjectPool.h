#pragma once
#include <functional>
#include <memory>
#include <stack>

namespace LightRuntime
{
    template <class TObject, auto... Args>
    class ObjectPool
    {
    public:
        ObjectPool() = default;
        ObjectPool(const ObjectPool&) = delete;

        std::unique_ptr<TObject>& Get()
        {
            TObject* objectPtr;
            if (pool.empty())
            {
                objectPtr = new TObject(Args...);
                objects[objectPtr] = std::unique_ptr<TObject>(objectPtr);
            }
            else
            {
                objectPtr = pool.top();
                pool.pop();
            }

            return objects[objectPtr];
        }

        void Release(std::unique_ptr<TObject>& element)
        {
            pool.push(element.get());
        }

    private:
        std::stack<TObject*> pool;
        std::unordered_map<TObject*, std::unique_ptr<TObject>> objects;
    };
}
