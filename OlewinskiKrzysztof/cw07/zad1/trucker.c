#include "belt.h"

#define NUMBER 305
#define NUM_OF_SEMAPHORES 4

int maxBeltCapacity;
int currentBeltCapacity;
int maxTruckCapacity;
int currentTruckCapacity;
int maxWeight;

key_t key;
int shmID = -1;
Belt *belt;
Box box;
int semID = -1;
int semTaken = 0;
int status;

void prepare_memory();
void prepare_semaphores();
void load_boxes();
void finish_work();
void unload_truck();
void int_handler(int signo);

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

      if (atexit(finish_work) == -1)
      {
         perror("Cannot set at exit operation");
         return -1;
      }

      signal(SIGINT, int_handler);

      prepare_memory();
      prepare_semaphores();
      load_boxes();
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
   belt = (Belt *) add;

   belt_init(maxWeight, maxBeltCapacity, belt);
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

void load_boxes()
{
   struct sembuf sops;
   sops.sem_flg = 0;

   while(1)
   {
      sops.sem_num = TRUCKER;
      sops.sem_op = -1;
      if ( semop(semID, &sops, 1) == - 1)
      {
         perror("Cannot take trucker semaphore");
         return;
      }

      sops.sem_num = BELT;
      sops.sem_op = -1;
      if ( semop(semID, &sops, 1) == - 1)
      {
         perror("Cannot take blet semaphore");
         return;
      }
      semTaken = 1;

      status = belt_pop(belt, &box);
      while(status == 0)
      {
         if (currentTruckCapacity == maxTruckCapacity)
         {
            printf("No free place - truck is going to be unloaded");
            unload_truck();
         }

         currentTruckCapacity++;
         printf("Box loaded on truck; weight: %d, pid: %d. Time from pop on belt to loaded on truck: %ld. Free places: %d\n",
         box.weight, box.pid, getMicroTime()-box.time, maxTruckCapacity-currentTruckCapacity);

         status = belt_pop(belt, &box);
      }

      sops.sem_num = BELT;
      sops.sem_op = 1;
      if ( semop(semID, &sops, 1) == - 1)
      {
         perror("Cannot give back blet semaphore");
         return;
      }
      semTaken = 0;

      sops.sem_num = TRUCKER;
      sops.sem_op = 1;
      if ( semop(semID, &sops, 1) == - 1)
      {
         perror("Cannot give back trucker semaphore");
         return;
      }
   }
}

void unload_truck()
{
   currentTruckCapacity = 0;
   printf("Truck had been unloaded and arrived empty");
}

void int_handler(int signo)
{
    exit(0);
}

void finish_work()
{
   if (!semTaken)
   {
      struct sembuf sops;
      sops.sem_flg = 0;
      sops.sem_num = BELT;
      sops.sem_op = -1;
      if ( semop(semID, &sops, 1) == - 1)
      {
         perror("Cannot take blet semaphore");
         return;
      }
      semTaken = 1;
   }

   status = belt_pop(belt, &box);
   while(status == 0)
   {
      if (currentTruckCapacity == maxTruckCapacity)
      {
         printf("No free place - truck is going to be unloaded");
         unload_truck();
      }

      currentTruckCapacity++;
      printf("Box loaded on truck; weight: %d, pid: %d. Time from pop on belt to loaded on truck: %ld. Free places: %d\n",
      box.weight, box.pid, getMicroTime()-box.time, maxTruckCapacity-currentTruckCapacity);

      status = belt_pop(belt, &box);
   }

   if (shmdt(belt) == -1)
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
