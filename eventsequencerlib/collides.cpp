#include "collides.h"

bool Collides::startAndLength(int start1, int length1, int start2, int length2)
{
    return (start1 < start2 ?
            (start1 + length1 > start2) :
            (start2 + length2 > start1));
}
