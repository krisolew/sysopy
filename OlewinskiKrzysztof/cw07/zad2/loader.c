#define _GNU_SOURCE
#include "belt.h"

Belt *belt = NULL;
key_t key;
int shmID = -1;
int semID = -1;
int boxWeight;
int cycleNumber;
int queue;
sem_t *trucker_sem;
sem_t *loaders_sem;
sem_t *belt_sem;

void prepare_memory();
void prepare_semaphores();
void finish_work(void);
void int_handler(int signo);
void put_box(Box box);
void write_pid();

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
    write_pid();

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
   if (sem_wait(loaders_sem)  == - 1)
   {
      perror("Cannot take loader semaphore");
      return;
   }

   if (sem_wait(belt_sem) == - 1)
   {
      perror("Cannot take belt semaphore");
      return;
   }


   box.time = get_micro_time();
   if(belt_push(belt, box) == -1)
   {
      printf("No free places on belt\n");
      cycleNumber++;

      if (sem_post(trucker_sem) == - 1)
      {
         perror("Cannot give back trucker semaphore");
         return;
      }

      if (sem_post(belt_sem) == - 1)
      {
         perror("Cannot give back blet semaphore");
         return;
      }
   }
   else
   {
      printf("Put box; size: %d time: %ld \n",box.weight, box.time);
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

void prepare_memory()
{
   shmID = shm_open("belt_memory", O_RDWR, 0666);
   if (shmID == -1)
   {
      perror("Cannot get shared memory");
      return;
   }

   void *add = mmap(NULL, sizeof(Belt), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
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
   trucker_sem = sem_open("trucker_sem", O_RDWR);
   if (trucker_sem == SEM_FAILED)
   {
      perror("Cannot create trucker semaphore");
      return;
   }

   loaders_sem = sem_open("loaders_sem", O_RDWR);
   if (loaders_sem == SEM_FAILED)
   {
      perror("Cannot create loader semaphore");
      return;
   }

   belt_sem = sem_open("belt_sem", O_RDWR);
   if (belt_sem == SEM_FAILED)
   {
      perror("Cannot create belt semaphore");
      return;
   }
}

void finish_work(void)
{
   if (munmap(belt, sizeof(belt)) == -1)
   {
      perror("Cannot detache shared memory");
      return;
   }

   if(sem_close(trucker_sem) == -1)
   {
      perror("Cannot detache trucker semaphores");
      return;
   }

   if(sem_close(loaders_sem) == -1)
   {
      perror("Cannot detache loader semaphores");
      return;
   }

   if(sem_close(belt_sem) == -1)
   {
      perror("Cannot detache belt semaphores");
      return;
   }
}

void write_pid(){
    if ((queue = mq_open("/pidQueue", O_WRONLY)) == -1)
    {
      perror("Cannot open queue");
      return;
    }

    char pid[10];
    sprintf(pid,"%d",getpid());
    if (mq_send(queue, pid, 10, 1) != 0)
    {
        perror("Cannot send pid");
    }

    if (mq_close(queue) == -1)
    {
      perror("Cannot close queue");
    }

}

void int_handler(int signo)
{
    exit(2);
}
