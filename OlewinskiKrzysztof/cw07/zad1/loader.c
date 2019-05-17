
#include "belt.h"

Belt *belt = NULL;
key_t key;
int shmID = -1;
int semID = -1;
int boxWeight;
int cycleNumber;

void prepare_memory();
void prepare_semaphores();
void finish_work(void);
void int_handler(int signo);
void put_box(Box box);

int main(int argc, char **argv)
{

   if (argc < 2)
   {
      printf("Expected at least 1 arguments\n");
      return -1;
   }

   boxWeight = strtol(argv[1], NULL, 10);
   if (boxWeight < 1)
   {
      printf("Box weight cannot be smaller than 0\n");
      return -1;
   }

   int cycles = 0;

   if(argc == 3){
      cycles = 1;
      cycleNumber = (int) strtol(argv[2], NULL, 10);
      if (cycleNumber < 1)
      {
         printf("Number of cycles cannot be smaller than 0\n");
         return -1;
      }
   }

    key = get_belt_key();
    prepare_memory();
    prepare_semaphores();

    if (atexit(finish_work) == -1)
    {
      perror("Cannot set at exit operation");
      return -1;
    }

    signal(SIGINT, int_handler);

    Box box;
    box.pid = getpid();
    box.weight = boxWeight;
    while(!cycles || (cycleNumber--)){
        put_box(box);
    }
    exit(0);
}

void put_box(Box box)
{
   struct sembuf sops;
   sops.sem_num = LOADERS;
   sops.sem_op = -1;
   sops.sem_flg = 0;
   if (semop(semID, &sops, 1) == -1)
   {
      perror("Cannot take loader semaphore");
      return;
   }

   sops.sem_num = BELT;
   if (semop(semID, &sops, 1) == -1)
   {
      perror("Cannot take belt semaphore");
      return;
   }

   box.time = get_micro_time();
   if(belt_push(belt, box) == -1)
   {
      printf("No free places on belt\n");
      cycleNumber++;

      sops.sem_num = TRUCKER;
      sops.sem_op = 1;
      if ( semop(semID, &sops, 1) == - 1)
      {
         perror("Cannot wake trucker");
         return;
      }
      sops.sem_num = BELT;
      sops.sem_op = 1;
      if ( semop(semID, &sops, 1) == - 1)
      {
         perror("Cannot give back belt semaphore");
         return;
      }
   }
   else
   {
      printf("Put box; size: %d time: %ld \n",box.weight, box.time);
      sops.sem_num = BELT;
      sops.sem_op = 1;
      if ( semop(semID, &sops, 1) == - 1)
      {
         perror("Cannot give backbelt semaphore");
         return;
      }
      sops.sem_num = LOADERS;
      sops.sem_op = 1;
      if ( semop(semID, &sops, 1) == - 1)
      {
         perror("Cannot give back loader semaphore");
         return;
      }
   }
}

void prepare_memory()
{
   shmID = shmget(key, 0, 0);
   if (shmID == -1)
   {
      perror("Cannot get shared memory");
      return;
   }

   void *add = shmat(shmID, NULL, 0);
   if (add == (void*) -1)
   {
      perror("Cannot attache shared memory");
      return;
   }
   belt = (Belt *) add;

   if (belt->maxWeight < boxWeight)
   {
      printf("Box is to heavy\n");
      exit(1);
   }
}

void prepare_semaphores()
{
   semID = semget(key, 0, 0);
   if (semID == -1)
   {
      perror("Cannot get semaphores");
      return;
   }
}

void finish_work(void)
{
   if (shmdt(belt) == -1)
   {
      perror("Cannot detache shared memory");
      return;
   }
}

void int_handler(int signo)
{
    exit(2);
}
