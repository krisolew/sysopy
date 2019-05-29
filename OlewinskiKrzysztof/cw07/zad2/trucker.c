#define _GNU_SOURCE
#include "belt.h"

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
int queue;

sem_t *trucker_sem;
sem_t *loaders_sem;
sem_t *belt_sem;

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
   key = get_belt_key();

   if ((shmID = shm_open("belt_memory", O_CREAT | O_EXCL | O_RDWR, 0666)) == -1)
   {
      perror("Cannot create shared memory");
      return;
   }

   if (ftruncate(shmID, sizeof(Belt)) == -1)
   {
      perror("Cannot set size of shared memory");
      return;
   }

   void *add = mmap(NULL, sizeof(Belt), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
   if (add == (void*) -1)
   {
      perror("Cannot attache shared memory");
      return;
   }
   belt = (Belt *) add;
   belt_init(maxWeight, maxBeltCapacity, belt);

   struct mq_attr queue_attr;
   queue_attr.mq_maxmsg = 10;
   queue_attr.mq_msgsize = 10;

   if ((queue = mq_open("/pidQueue",O_CREAT | O_EXCL | O_RDONLY | O_NONBLOCK, 0666, &queue_attr)) == -1){
      perror("Canot create queue");
      return;
   }
}

void prepare_semaphores()
{
   trucker_sem = sem_open("trucker_sem", O_CREAT | O_EXCL | O_RDWR, 0666, 0);
   if (trucker_sem == SEM_FAILED)
   {
      perror("Cannot create trucker semaphore");
      return;
   }

   loaders_sem = sem_open("loaders_sem",O_CREAT | O_EXCL | O_RDWR, 0666, 1);
   if (loaders_sem == SEM_FAILED)
   {
      perror("Cannot create loader semaphore");
      return;
   }

   belt_sem = sem_open("belt_sem",O_CREAT | O_EXCL | O_RDWR, 0666, 1);
   if (belt_sem == SEM_FAILED)
   {
      perror("Cannot create belt semaphore");
      return;
   }
}

void load_boxes()
{
   while(1)
   {
      if (sem_wait(trucker_sem)  == - 1)
      {
         perror("Cannot take trucker semaphore");
         return;
      }

      if (sem_wait(belt_sem) == - 1)
      {
         perror("Cannot take belt semaphore");
         return;
      }

      status = belt_pop(belt, &box);
      while(status == 0)
      {
         if (currentTruckCapacity == maxTruckCapacity)
         {
            printf("No free place - truck is going to be unloaded\n");
            unload_truck();
         }

         currentTruckCapacity++;
         printf("Box loaded on truck; weight: %d, pid: %d. Time from pop on belt to loaded on truck: %ld. Free places: %d\n",
         box.weight, box.pid, get_micro_time()-box.time, maxTruckCapacity-currentTruckCapacity);

         status = belt_pop(belt, &box);
      }

      if (sem_post(belt_sem) == - 1)
      {
         perror("Cannot give back blet semaphore");
         return;
      }

      if (sem_post(loaders_sem) == - 1)
      {
         perror("Cannot give back loader semaphore");
         return;
      }
   }
}

void unload_truck()
{
   currentTruckCapacity = 0;
   printf("Truck had been unloaded and arrived empty\n");
}

void int_handler(int signo)
{
    exit(0);
}

void finish_work()
{
   char pid[10];
   int i;
   while(mq_receive(queue, pid, 10, NULL) != -1){
      i = strtol(pid,NULL,10);
      kill(i, SIGINT);
   }

   if (mq_close(queue) == -1)
   {
      perror("Cannot close queue");
      return;
   }
   if (mq_unlink("/pidQueue") == -1)
   {
      perror("Cannot remove queue");
      return;
   }

   if (belt_sem != NULL && belt != NULL)
   {
      status = belt_pop(belt, &box);
      while(status == 0)
      {
         if (currentTruckCapacity == maxTruckCapacity)
         {
            printf("No free place - truck is going to be unloaded\n");
            unload_truck();
         }

         currentTruckCapacity++;
         printf("Box loaded on truck; weight: %d, pid: %d. Time from pop on belt to loaded on truck: %ld. Free places: %d\n",
         box.weight, box.pid, get_micro_time()-box.time, maxTruckCapacity-currentTruckCapacity);

         status = belt_pop(belt, &box);
      }
   }

   if (munmap(belt, sizeof(belt)) == -1)
   {
      perror("Cannot detache shared memory");
      return;
   }

   if (shm_unlink("belt_memory") == -1)
   {
      perror("Cannot delete shared memory");
      return;
   }
   if(sem_close(trucker_sem) == -1)
   {
      perror("Cannot detache trucker semaphores");
      return;
   }
   if (sem_unlink("trucker_sem") == -1)
   {
      perror("Cannot delete trucker semaphores");
      return;
   }

   if(sem_close(loaders_sem) == -1)
   {
      perror("Cannot detache loader semaphores");
      return;
   }
   if (sem_unlink("loaders_sem") == -1)
   {
      perror("Cannot delete loader semaphores");
      return;
   }

   if(sem_close(belt_sem) == -1)
   {
      perror("Cannot detache belt semaphores");
      return;
   }
   if (sem_unlink("belt_sem") == -1)
   {
      perror("Cannot delete belt semaphores");
      return;
   }
}
