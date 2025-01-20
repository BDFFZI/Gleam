#include "Segment.h"

namespace Light
{
    float Segment::GetLength() const
    {
        return distance(positionA, positionB);
    }
}