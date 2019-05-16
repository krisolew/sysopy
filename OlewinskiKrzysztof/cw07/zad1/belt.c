#include "belt.h"

void belt_init(int max_weight, int max_capacity, Belt *belt)
{
   belt->curWeight = 0;
   belt->curCapacity = 0;
   belt->maxCapacity = max_capacity;
   belt->maxWeight = max_weight;
   belt->head = -1;
   belt->tail = 0;
}

int belt_is_empty(Belt *belt)
{
   if (belt->head == -1) return 1;
   else return 0;
}

int belt_is_full(Belt *belt)
{
   if (belt->maxCapacity == belt->curCapacity) return 1;
   else return 0;
}

int belt_push(Belt *belt, Box box)
{
   if ( belt_is_full(belt) || belt->curWeight + box.weight >= belt->maxWeight)
   {
      return -1;
   }

   if (belt_is_empty(belt) == 1)
   {
      belt->head = belt->tail = 0;
   }

   belt->fifo[belt->tail++] = box;
   belt->curWeight += box.weight;
   belt->curCapacity++;

   if (belt->tail == belt->maxCapacity) belt->tail = 0;
   return 0;
}

int belt_pop(Belt *belt, Box *box)
{
   if (belt_is_empty(belt))
   {
      return -1;
   }

   box = &belt->fifo[belt->head++];
   belt->curWeight -= box->weight;
   belt->curCapacity--;

   if (belt->head == belt->maxCapacity) belt->head = 0;
   if (belt->head == belt->tail) belt->head = -1;

   return 0;
}

long get_micro_time()
{
    struct timespec marker;
    clock_gettime(CLOCK_MONOTONIC, &marker);
    return marker.tv_nsec / 1000;
}

key_t get_belt_key(){
   char *path = getenv("HOME");
   key_t key;
   if ((key = ftok(path, NUMBER)) == -1)
   {
      perror("Cannot create key");
      return -1;
   }
   return key;
}
