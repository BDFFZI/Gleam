#pragma once

namespace LightRuntime
{
    struct rect
    {
        float x;
        float y;
        float width;
        float height;
    };

    struct color
    {
        static color white;
        static color black;
        static color red;
        static color green;
        static color blue;

        float r;
        float g;
        float b;
        float a;
    };
}
