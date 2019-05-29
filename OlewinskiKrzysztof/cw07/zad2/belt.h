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
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define NUMBER 370

typedef enum semType{
    TRUCKER = 0, BELT = 1, LOADERS = 2
}semType;

typedef struct Box{
    int weight;
    pid_t pid;
    long time;
}Box;

typedef struct Belt
{
   int curWeight;
   int curCapacity;
   int maxWeight;
   int maxCapacity;
   int head;
   int tail;
   Box fifo[1024];
}Belt;

typedef struct pid_table{
    pid_t pids[1024];
    int size;
}pid_table;

void belt_init(int max_weight, int max_capacity, Belt *belt);
int belt_is_empty(Belt *belt);
int belt_is_full(Belt *belt);
int belt_pop(Belt *belt, Box *box);
int belt_push(Belt *belt, Box box);
long get_micro_time();
key_t get_belt_key();
