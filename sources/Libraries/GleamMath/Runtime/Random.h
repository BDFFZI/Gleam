﻿#pragma once
#include "LinearAlgebra/Vector.h"


namespace Gleam
{
    class Random
    {
    public:
        static float Range(float min, float max);
        static int Range(int min, int max);
        /**
         * 返回一个0-1的小数
         */
        static float Value();
        /**
         * 返回一个随机颜色值
         * @param alphaMin 
         * @param alphaMax 
         * @return 
         */
        static float4 ColorHSV(float alphaMin = 1, float alphaMax = 1);
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