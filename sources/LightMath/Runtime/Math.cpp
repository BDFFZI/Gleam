#include "Math.h"

void CombineHash(size_t& seed, size_t hash)
{
    //复制的glm的

    hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= hash;
}
