#include <array>
#include <gtest/gtest.h>

#include "LightUtility/Runtime/ObjectPool.h"

using namespace LightRuntime;


TEST(Utility, ObjectPool)
{
    ObjectPool<std::array<int, 3>, std::array{1, 2, 3}> objPool;
    auto& array = *objPool.Get();
    ASSERT_EQ(array[0], 1);
    ASSERT_EQ(array[1], 2);
    ASSERT_EQ(array[2], 3);
    objPool.Release(&array);

    struct Object
    {
        Object(const int a, const int b, const int& c)
        {
            sum = a + b + c;
        }

        constexpr static int b()
        {
            return 2;
        }

        int sum;
    };

    ObjectPool<Object, 1, Object::b(), 3> objPool2;
    Object& object = *objPool2.Get();
    ASSERT_EQ(object.sum, 6);
    objPool2.Release(&object);
}
