#include <gtest/gtest.h>

#include "LightMath/Runtime/Math.h"
#include "LightMath/Runtime/Vector3.h"


TEST(Math, Cross)
{
    Vector3 a(1, 2, 3);
    Vector3 b(3, 7, 5);
    ASSERT_EQ(Vector3::Cross(a,b), Vector3(-11, 4, 1));
}

TEST(Math, Rotate)
{
    Vector3 normal = Vector3::Normalize({1, 1, 1});
    Vector3 vector = {3, 0, 0};
    ASSERT_LE(Vector3::Magnitude(Vector3::Rotate(vector, normal,45 * Deg2Rad) - Vector3(2.41f, 1.52f, -0.93f)), 0.01f);
}
    