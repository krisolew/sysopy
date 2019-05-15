#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/time.h>


int maxBeltCapacity;
int currentBeltCapacity;
int maxTruckCapacity;
int currentTruckCapacity;
int maxWeight;

int main(int argc, char *argv[])
{
      if (argc < 4)
      {
         printf("Expected 4 arguments\n");
         return -1;
      }

      maxTruckCapacity = strtol(argv[1], NULL, 10);
      if (maxTruckCapacity < 1)
      {
         printf("Track capacity cannot be smaller than 0\n");
         return -1;
      }

      maxBeltCapacity = strtol(argv[2], NULL, 10);
      if (maxBeltCapacity < 1)
      {
         printf("Belt capacity cannot be smaller than 0\n");
         return -1;
      }

      maxWeight = strtol(argv[3], NULL, 10);
      if (maxWeight < 1)
      {
         printf("Weight cannot be smaller than 0\n");
         return -1;
      }

      printf("%d %d %d\n", maxTruckCapacity, maxBeltCapacity, maxWeight);
}
