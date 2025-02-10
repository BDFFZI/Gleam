#include "Segment.h"

#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

namespace Gleam
{
    float Segment::GetLength() const
    {
        return distance(positionA, positionB);
    }
}