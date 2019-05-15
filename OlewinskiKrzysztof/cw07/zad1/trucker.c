#include "belt.h"

#define NUMBER 304
#define NUM_OF_SEMAPHORES 4

int maxBeltCapacity;
int currentBeltCapacity;
int maxTruckCapacity;
int currentTruckCapacity;
int maxWeight;

key_t key;
int shmID = -1;
Belt *fifo;
int semID = -1;

void prepare_memory();
void prepare_semaphores();
void finish_work();

int main(int argc, char *argv[])
{
      if (argc < 4)
      {
         printf("Expected 3 arguments\n");
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

      prepare_memory();

      prepare_semaphores();

      finish_work();
}

void prepare_memory()
{
   char *path = getenv("HOME");

   if ((key = ftok(path, NUMBER)) == -1)
   {
      perror("Cannot create key");
      return;
   }

   if ((shmID = shmget(key, sizeof(Belt), IPC_CREAT | IPC_EXCL | 0666)) == -1)
   {
      perror("Cannot create shared memory");
      return;
   }

   void *add = shmat(shmID, NULL, 0);
   if (add == (void*) -1)
   {
      perror("Cannot attache shared memory");
      return;
   }
   fifo = (Belt *) add;

   beltInit(maxWeight, maxBeltCapacity, fifo);
}

void prepare_semaphores()
{
   if((semID = semget(key, NUM_OF_SEMAPHORES, IPC_CREAT | IPC_EXCL | 0666)) == -1)
   {
      perror("Cannot create semaphores");
      return;
   }

   int i = 1;
   for (; i < 3; i++)
   {
      if (semctl(semID, i, SETVAL, 1) == -1)
      {
         perror("Cannot set semaphore");
         return;
      }
   }

   if (semctl(semID, TRUCKER, SETVAL, 0) == -1)
   {
      perror("Cannot set semaphore");
      return;
   }
}

void finish_work()
{
   if (shmdt(fifo) == -1)
   {
      perror("Cannot detache shared memory");
      return;
   }

   if (shmctl(shmID, IPC_RMID, NULL) == -1)
   {
      perror("Cannot delete shared memory");
      return;
   }

   if (semctl(semID, 0, IPC_RMID) == -1)
   {
      perror("Cannot delete semaphores");
      return;
   }
}
