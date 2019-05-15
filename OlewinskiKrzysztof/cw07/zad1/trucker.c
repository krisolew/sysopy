#include "belt.h"

#define NUMBER 303

int maxBeltCapacity;
int currentBeltCapacity;
int maxTruckCapacity;
int currentTruckCapacity;
int maxWeight;

key_t key;
int shmID = -1;

void prepare_memory();

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

      prepare_memory();
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
      perror("Cannot create memory");
      return;
   }

   void *add = shmat(shmID, NULL, 0);
   if (add == (void*) -1)
   {
      perror("Cannot add memory");
      return;
   }

   beltInit(maxWeight, maxBeltCapacity);

}
