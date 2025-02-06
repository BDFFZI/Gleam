#include <array>
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "LightUtility/Runtime/Timer.h"
#include "LightUtility/Runtime/Pool/ObjectPool.h"
#include "LightUtility/Runtime/Pool/ThreadPool.h"

using namespace Light;

TEST(Utility, ObjectPool)
{
    ObjectPool<std::array<int, 3>, std::array{1, 2, 3}> objPool;
    auto& array = objPool.Get();
    ASSERT_EQ(array.at(0), 1);
    ASSERT_EQ(array.at(1), 2);
    ASSERT_EQ(array.at(2), 3);
    objPool.Release(array);


    static int recycleCount = 0;
    struct Object
    {
        Object(const int a, const int b, const int& c)
        {
            sum = a + b + c;
        }
        ~Object()
        {
            recycleCount++;
        }

        constexpr static int b()
        {
            return 2;
        }

        int sum;
    };

    {
        ObjectPool<Object, 1, Object::b(), 3> objPool2;
        auto& object = objPool2.Get();
        auto& object2 = objPool2.Get();
        ASSERT_EQ(object.sum, 6);
        objPool2.Release(object);
        objPool2.Release(object2);
    }
    ASSERT_EQ(recycleCount, 2);
}

TEST(Utility, Worker)
{
    Worker worker = Worker();
    worker.Execute(
        [] { std::cout << "执行1\n"; },
        [] { std::cout << "完成1\n"; }
    );
    worker.Execute(
        []
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "执行2\n";
        },
        [] { std::cout << "完成3\n"; }
    );
    worker.Execute(
        [] { std::cout << "执行3\n"; },
        [] { std::cout << "完成3\n"; }
    );

    std::cout << "布置完成\n" << std::flush;
}

TEST(Utility, ThreadPool)
{
    ThreadPool pool;

    pool.Schedule([]
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }, [] { std::cout << "Finish Task 1\n" << std::flush; });
    pool.Schedule([]
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }, [] { std::cout << "Finish Task 2\n" << std::flush; });
    pool.Schedule([]
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }, [] { std::cout << "Finish Task 3\n" << std::flush; });

    std::cout << "Submit Finished\n" << std::flush;

    ASSERT_EQ(pool.GetThreadCount(), 3);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    pool.Schedule([]
    {
    }, [] { std::cout << "Finish Task 4\n" << std::flush; });

    ASSERT_EQ(pool.GetThreadCount(), 3);
}

TEST(Utility, Chronograph)
{
    Timer chronograph;
    chronograph.Tick();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    std::cout << chronograph.Tick() << '\n' << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << chronograph.Tick() << '\n' << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << chronograph.Tick() << '\n' << std::flush;
}