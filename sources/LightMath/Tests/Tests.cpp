#include <format>
#include <gtest/gtest.h>

#include "LightMath/Runtime/VectorMath.hpp"

/// 验证工具：https://www.math666.com/

TEST(Math, Swizzle)
{
    float3 vector3 = 0;
    vector3.xy = 1; //{1,1,0}
    vector3.yz += vector3.xy * 2; //{1,3,2}
    ++vector3.zx; //{2,3,3}
    ASSERT_TRUE(all(vector3++ == float3(2, 3, 3)));

    float2 vector2 = vector3.zx;
    ASSERT_TRUE(all(vector2 == float2(4,3)));

    ASSERT_EQ(to_string(vector3.zyx), "(4.000000,4.000000,3.000000)");
}

// TEST(Math, Vector3)
// {
//     float3 vector(1, 2, 3);
//     ASSERT_FLOAT_EQ(length_sq(vector), 14);
//     ASSERT_FLOAT_EQ(length(vector), 3.741657f);
//     ASSERT_EQ(to_string(normalize( vector)), "(0.267261,0.534522,0.801784)");
//     
//     float3 vector2(6, 4, 5);
//     ASSERT_EQ(to_string(lerp(vector,vector2,0.5f)), "(3.500000,3.000000,4.000000)");
//     ASSERT_FLOAT_EQ(dot(vector,vector2), 29);
//     ASSERT_EQ(to_string(cross(vector,vector2)), "(-2.000000,13.000000,-8.000000)");
//     ASSERT_FLOAT_EQ(angle(vector,vector2), 27.96183f);
//     ASSERT_EQ(to_string(project(vector,normalize(vector2))), "(2.259740,1.506494,1.883117)");
//     ASSERT_EQ(to_string(rotate(vector,normalize(vector2),90.0f)), "(2.487662,0.025006,2.794801)");
// }


// TEST(Math, Matrix3x3)
// {
//     Matrix3x3 matrix = {
//         17, 7, 8,
//         10, 11, 12,
//         9, 5, 1
//     };
//
//     ASSERT_FLOAT_EQ(matrix.GetDeterminant(), -539);
//
//     Matrix3x3 matrix2 = {
//         4, 1, 8,
//         1, 2, 7,
//         6, 6, 4
//     };
//     ASSERT_EQ(matrix*matrix2, Matrix3x3(
//                   123,79,217,
//                   123,104,205,
//                   47,25,111
//               ));
// }
//
// TEST(Math, Matrix4x4)
// {
//     Matrix4x4 matrix = {
//         1, 5, 9, 13,
//         2, 6, 10, 14,
//         3, 7, 11, 15,
//         4, 8, 12, 16
//     };
//
//     ASSERT_FLOAT_EQ(matrix.GetElement(2,2), 11);
//     ASSERT_EQ(to_string(matrix.GetColumn(2)), to_string(float4(9,10,11,12)));
//     ASSERT_EQ(to_string(matrix.GetRow(2)), to_string(float4(3,7,11,15)));
//     matrix.SetElement(1, 1, 17);
//     matrix.SetColumn(3, float4(13, 9, 5, 1));
//     matrix.SetRow(0, float4(16, 15, 14, 13));
//     ASSERT_EQ(to_string(matrix), R"(|16.000000 15.000000 14.000000 13.000000|
// |2.000000 17.000000 10.000000 9.000000|
// |3.000000 7.000000 11.000000 5.000000|
// |4.000000 8.000000 12.000000 1.000000|)");
//
//     ASSERT_EQ(matrix.GetAdjoint(), Matrix4x4(
//                   -539,325,912,-478,
//                   0,-650,1300,-650,
//                   187,325,-1046,-126,
//                   -88,0,-1496,1474
//               ));
//     ASSERT_FLOAT_EQ(matrix.GetDeterminant(), -7150);
//
//     ASSERT_EQ(to_string(matrix.GetInverse()), R"(|0.075385 -0.045455 -0.127552 0.066853|
// |0.000000 0.090909 -0.181818 0.090909|
// |-0.026154 -0.045455 0.146294 0.017622|
// |0.012308 -0.000000 0.209231 -0.206154|)");
//
//     Matrix4x4 matrix3 = Matrix3x3{
//         1, 5, 9,
//         2, 6, 10,
//         3, 7, 11,
//     };
//     ASSERT_EQ(matrix3, Matrix4x4(
//                   1,5,9,0,
//                   2,6,10,0,
//                   3,7,11,0,
//                   0,0,0,1
//               ));
// }
//
// TEST(Math, TRSMatrix)
// {
//     ASSERT_EQ(Matrix4x4::Scale({1,2,3}), Matrix4x4(
//                   1, 0, 0, 0,
//                   0, 2, 0, 0,
//                   0, 0, 3, 0,
//                   0, 0, 0, 1
//               ));
//
//     ASSERT_EQ(Matrix4x4::Translate(float3(1,2,3)), Matrix4x4(
//                   1, 0, 0, 1,
//                   0, 1, 0, 2,
//                   0, 0, 1, 3,
//                   0, 0, 0, 1
//               ));
//
//
//     ASSERT_EQ(to_string(Matrix4x4::Rotate({30, 60,90})), R"(|0.433013 -0.500000 0.750000 0.000000|
// |0.866025 -0.000000 -0.500000 0.000000|
// |0.250000 0.866025 0.433013 0.000000|
// |0.000000 0.000000 0.000000 1.000000|)");
//
//     ASSERT_EQ(
//         Matrix4x4::TRS(
//             {2,2,2},
//             {0,-90,0},
//             float3::one
//         ).GetInverse() * float4(0, 0, 0, 1), float4(-2,-2,2,1));
// }
//
// TEST(Math, Project)
// {
//     ASSERT_EQ(to_string(Matrix4x4::Ortho(8,5,0.3f,1000.0f)), R"(|0.125000 0.000000 0.000000 0.000000|
// |0.000000 0.200000 0.000000 0.000000|
// |0.000000 0.000000 0.001000 -0.000300|
// |0.000000 0.000000 0.000000 1.000000|)");
//
//     ASSERT_EQ(to_string(Matrix4x4::Perspective(60.0f,16.0f/9.0f,0.3f,1000.0f)), R"(|0.974279 0.000000 0.000000 0.000000|
// |0.000000 1.732051 0.000000 0.000000|
// |0.000000 0.000000 1.000300 -0.300090|
// |0.000000 0.000000 1.000000 0.000000|)");
// }
