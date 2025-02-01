#include "Segment.h"

#include "LightMath/Runtime/LinearAlgebra/VectorMath.h"

namespace Light
{
    float Segment::GetLength() const
    {
        return distance(positionA, positionB);
    }
}