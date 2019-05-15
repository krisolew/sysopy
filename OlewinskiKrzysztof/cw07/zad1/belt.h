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

typedef struct Box{
    int weight;
    pid_t pid;
    long time;
}Box;

typedef struct Belt
{
   int curWeight;
   int curQuantity;
   int maxWeight;
   int maxCapacity;
   int head;
   int tail;
   Box fifo[1024];
}Belt;

void beltInit(int max_weight, int max_capacity);
