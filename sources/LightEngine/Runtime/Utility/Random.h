#pragma once
#include "LightMath/Runtime/Vector.hpp"
#include "LightMath/Runtime/Graphics/Color.hpp"

namespace Light
{
    class Random
    {
    public:
        static float Range(float min, float max);
        static int Range(int min, int max);
        /**
         * 返回一个0-1的小数
         */
        static float Value() { return Range(0.0f, 1.0f); }
        /**
         * 返回一个随机颜色值
         * @param alphaMin 
         * @param alphaMax 
         * @return 
         */
        static Color ColorHSV(float alphaMin = 1, float alphaMax = 1);
        /**
         * 返回一个半径为1的球的表面坐标
         */
        static float3 OnUnitSphere();
        /**
         * 返回一个半径为1的球的内部坐标
         */
        static float3 InsideUnitSphere();

        static unsigned int seed;
    };
}