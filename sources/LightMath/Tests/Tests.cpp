#include <gtest/gtest.h>

#include "LightMath/Runtime/Vector3.h"


TEST(Math, ALL)
{
    Vector3 a(1, 2, 3);
    Vector3 b(3, 7, 5);
    ASSERT_EQ(Vector3::Cross(a,b), Vector3(-11, 4, 1));
}
