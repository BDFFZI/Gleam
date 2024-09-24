#include "Math.h"

#include <sstream>


namespace LightRuntime
{
#define float2Operation(operation) float2 operator##operation(const float2 left, const float2 right)\
    {\
        return {\
            left.x operation right.x,\
            left.y operation right.y,\
        };\
    }
    float2Operation(+)
    float2Operation(-)
    float2Operation(*)
    float2Operation(/)


    std::stringstream stringStream;
    std::string to_string(const float2 value)
    {
        stringStream << '(' << value.x << ',' << value.y << ')';
        std::string string = stringStream.str();
        stringStream.str("");
        return string;
    }
}
