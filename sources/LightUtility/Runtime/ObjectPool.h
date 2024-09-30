#pragma once
#include <functional>
#include <stack>

namespace LightRuntime
{
    template <class TObject, auto... Args>
    class ObjectPool
    {
    public:
        ObjectPool() = default;
        ObjectPool(const ObjectPool&) = delete;
        ~ObjectPool()
        {
            while (!pool.empty())
            {
                delete pool.top();
                pool.pop();
            }
        }

        TObject* Get()
        {
            TObject* object;
            if (pool.empty())
                object = new TObject(Args...);
            else
            {
                object = pool.top();
                pool.pop();
            }

            return object;
        }

        void Release(TObject* element)
        {
            pool.push(element);
        }

    private:
        std::stack<TObject*> pool;
    };
}
