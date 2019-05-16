#include "belt.h"

void belt_init(int max_weight, int max_capacity, Belt *belt)
{

}

int pop_from_belt(Belt *belt, Box *box)
{
   return 0;
}

long getMicroTime()
{
    struct timespec marker;
    clock_gettime(CLOCK_MONOTONIC, &marker);
    return marker.tv_nsec / 1000;
}
