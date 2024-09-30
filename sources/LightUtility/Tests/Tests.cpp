#include <array>
#include <gtest/gtest.h>
#include <condition_variable>

#include "LightUtility/Runtime/ObjectPool.h"

using namespace LightRuntime;


TEST(Utility, ObjectPool)
{
    ObjectPool<std::array<int, 3>, std::array{1, 2, 3}> objPool;
    auto& array = objPool.Get();
    ASSERT_EQ(array->at(0), 1);
    ASSERT_EQ(array->at(1), 2);
    ASSERT_EQ(array->at(2), 3);
    objPool.Release(array);

    struct Object
    {
        Object(const int a, const int b, const int& c)
        {
            sum = a + b + c;
        }
        ~Object()
        {
            std::cout << "回收成功\n";
        }

        constexpr static int b()
        {
            return 2;
        }

        int sum;
    };

    ObjectPool<Object, 1, Object::b(), 3> objPool2;
    auto& object = objPool2.Get();
    auto& object2 = objPool2.Get();
    ASSERT_EQ(object->sum, 6);
    objPool2.Release(object);
}

TEST(Utility, ThreadPool)
{
    std::mutex mutex;

    
    std::unique_lock lock(mutex);

    std::jthread thread1 = std::jthread([&]
    {
        // std::condition_variable conditionVariable ;
        // conditionVariable.wait()

        std::unique_lock lock(mutex);
    });
    
   
}
