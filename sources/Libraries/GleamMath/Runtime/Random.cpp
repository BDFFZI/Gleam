#include "Random.h"
#include <random>
#include <ctime>

#include "LinearAlgebra/VectorMath.h"

namespace Gleam
{
    unsigned int Random::seed = static_cast<unsigned int>(time(nullptr));

    float Random::Range(const float min, const float max)
    {
        seed++;
        std::default_random_engine engine(seed);
        std::uniform_real_distribution random(min, max);
        return random(engine);
    }
    int Random::Range(const int min, const int max)
    {
        seed++;
        std::default_random_engine engine(seed);
        std::uniform_int_distribution random(min, max);
        return random(engine);
    }
    float4 Random::ColorHSV(const float alphaMin, const float alphaMax)
    {
        return float4{
            Value(),
            Value(),
            Value(),
            Range(alphaMin, alphaMax),
        };
    }
    float3 Random::OnUnitSphere()
    {
        return normalize(float3{
            Value(),
            Value(),
            Value(),
        });
    }
    float3 Random::InsideUnitSphere()
    {
        const float3 surface = OnUnitSphere();
        return lerp(-surface, surface, Value());
    }
}