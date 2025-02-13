#include <gtest/gtest.h>
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamMath/Runtime/Geometry/Solid/Segment.h"

/// 验证工具：https://www.math666.com/

using namespace Gleam;

TEST(Math, Segement)
{
    Segment line = {0, 1};
    ASSERT_TRUE(equal(line.GetLength(),1.7320508f));
}

TEST(Math, Vector)
{
    float3 vector3 = 0;
    vector3.xy = 1; //{1,1,0}
    vector3.yz += vector3.xy * 2; //{1,3,2}
    ++vector3.zx; //{2,3,3}
    ASSERT_TRUE(all(vector3++ == float3(2, 3, 3)));

    float2 vector2 = vector3.zx;
    ASSERT_TRUE(all(vector2 == float2(4,3)));

    ASSERT_EQ(to_string(vector3.zyx), "(4.000000,4.000000,3.000000)");

    ASSERT_EQ(to_string(-vector3.zyx), "(-4.000000,-4.000000,-3.000000)");
    ASSERT_EQ(to_string(-vector3), "(-3.000000,-4.000000,-4.000000)");
}

TEST(Math, VectorMath)
{
    float3 vector(1, 2, 3);
    ASSERT_FLOAT_EQ(lengthsq(vector), 14);
    ASSERT_FLOAT_EQ(length(vector), 3.741657f);
    ASSERT_EQ(to_string(normalize( vector)), "(0.267261,0.534522,0.801784)");

    float3 vector2(6, 4, 5);
    ASSERT_EQ(to_string(lerp(vector,vector2,0.5f)), "(3.500000,3.000000,4.000000)");
    ASSERT_FLOAT_EQ(dot(vector,vector2), 29);
    ASSERT_EQ(to_string(cross(vector,vector2)), "(-2.000000,13.000000,-8.000000)");
    ASSERT_FLOAT_EQ(angle(vector,vector2), 27.96183f);
    ASSERT_EQ(to_string(project(vector,normalize(vector2))), "(2.259740,1.506494,1.883117)");
    ASSERT_EQ(to_string(rotate(vector,normalize(vector2),90.0f)), "(2.487662,0.025006,2.794801)");
}

TEST(Math, Matrix)
{
    float4x4 matrix = 0;
    matrix[1][1] = 1;
    matrix[2].z = matrix[1][1] + 1;
    matrix._m33 = matrix[2].z + 1;
    ASSERT_EQ(to_string(matrix._22_33_44), "(1.000000,2.000000,3.000000)");
}

TEST(Math, MatrixMath)
{
    float4x4 matrix = {
        2, 1, 1, 4,
        5, 5, 7, 7,
        7, 1, 3, 1,
        9, 7, 1, 7
    };

    float determinantF = determinant(matrix);
    ASSERT_FLOAT_EQ(determinantF, -800);

    float4x4 transposeM = transpose(matrix);
    ASSERT_EQ(to_string(transposeM), R"(|2.000000 5.000000 7.000000 9.000000|
|1.000000 5.000000 1.000000 7.000000|
|1.000000 7.000000 3.000000 1.000000|
|4.000000 7.000000 1.000000 7.000000|)");

    int sign;
    float3x3 cofactorM = cofactor(matrix, 1, 2, &sign);
    ASSERT_EQ(to_string(cofactorM), R"(|2.000000 1.000000 4.000000|
|7.000000 1.000000 1.000000|
|9.000000 7.000000 7.000000|)");
    ASSERT_EQ(sign, -1);

    float determinantF2 = determinant(cofactorM);
    ASSERT_FLOAT_EQ(determinantF2, 120);

    float cofactorDeterminant = cofactor_determinant(matrix, 1, 2);
    ASSERT_FLOAT_EQ(cofactorDeterminant, -120);

    float4x4 cofactorMatrix = cofactor_matrix(matrix);
    ASSERT_EQ(to_string(cofactorMatrix), R"(|-40.000000 320.000000 80.000000 -280.000000|
|60.000000 -80.000000 -120.000000 20.000000|
|-120.000000 120.000000 -40.000000 40.000000|
|-20.000000 -120.000000 80.000000 20.000000|)");

    float4x4 adjointM = adjoint(matrix);
    ASSERT_EQ(to_string(adjointM), R"(|-40.000000 60.000000 -120.000000 -20.000000|
|320.000000 -80.000000 120.000000 -120.000000|
|80.000000 -120.000000 -40.000000 80.000000|
|-280.000000 20.000000 40.000000 20.000000|)");

    float4x4 inverseM = inverse(matrix);
    ASSERT_EQ(to_string(inverseM), R"(|0.050000 -0.075000 0.150000 0.025000|
|-0.400000 0.100000 -0.150000 0.150000|
|-0.100000 0.150000 0.050000 -0.100000|
|0.350000 -0.025000 -0.050000 -0.025000|)");
}

TEST(Math, TRSMatrix)
{
    ASSERT_TRUE(all(float4x4::Scale({1,2,3}) ==float4x4(
        1, 0, 0, 0,
        0, 2, 0, 0,
        0, 0, 3, 0,
        0, 0, 0, 1
    ) ));

    ASSERT_TRUE(all(float4x4::Translate(float3(1,2,3))== float4x4(
        1, 0, 0, 1,
        0, 1, 0, 2,
        0, 0, 1, 3,
        0, 0, 0, 1
    )));


    ASSERT_EQ(to_string(float4x4::Rotate({30, 60,90})), R"(|0.433013 -0.500000 0.750000 0.000000|
|0.866025 -0.000000 -0.500000 0.000000|
|-0.500000 0.866025 0.433013 0.000000|
|0.000000 0.000000 0.000000 1.000000|)");

    float4x4 matrix = inverse(float4x4::TRS(
        {2, 2, 2},
        {0, -90, 0},
        1
    ));

    ASSERT_TRUE(all(mul(matrix,float4(0, 0, 0, 1)) == float4(-2,-2,2,1)));
}

TEST(Math, ProjectMatrix)
{
    ASSERT_EQ(to_string(float4x4::Ortho(8,5,0.3f,1000.0f)), R"(|0.125000 0.000000 0.000000 0.000000|
|0.000000 0.200000 0.000000 0.000000|
|0.000000 0.000000 0.001000 -0.000300|
|0.000000 0.000000 0.000000 1.000000|)");

    ASSERT_EQ(to_string(float4x4::Perspective(60.0f,16.0f/9.0f,0.3f,1000.0f)), R"(|0.974279 0.000000 0.000000 0.000000|
|0.000000 1.732051 0.000000 0.000000|
|0.000000 0.000000 1.000300 -0.300090|
|0.000000 0.000000 1.000000 0.000000|)");
}