#include "belt.h"

int main(int argc, char **argv)
{
    int cycleNumber;
    int cycles = 0;
    if (argc < 3)
    {
      printf("Expected at least 2 arguments\n");
      return -1;
    }
    int workerNumber = (int) strtol(argv[1], NULL, 10);
    if (workerNumber < 1)
    {
      printf("Worker number cannot be smaller than 0\n");
      return -1;
    }

    int boxWeight = (int) strtol(argv[2], NULL, 10);
    if (boxWeight < 1)
    {
      printf("Box weight cannot be smaller than 0\n");
      return -1;
    }

    if(argc == 4){
      cycles = 1;
      cycleNumber = (int) strtol(argv[3], NULL, 10);
      if (cycleNumber < 1)
      {
         printf("Number of cycles cannot be smaller than 0\n");
         return -1;
      }
    }


    int i,n,status;

    char nstring[100];
    char cyclestring[100];
    int pid[workerNumber];

    for(i = 0; i < workerNumber; i++){
        pid[i] = fork();
        if(pid[i] == 0){
            srand(time(NULL) + getpid());
            n = (rand() % boxWeight) + 1;
            sprintf(nstring,"%d",n);
            if(cycles){
                sprintf(cyclestring,"%d",cycleNumber);
                if(execlp("./loader","./loader", nstring, cyclestring, NULL) == -1)
                {
                   perror("Cannot create loader");
                   exit(-1);
                }
            }
            else{
                if(execlp("./loader","./loader", nstring, NULL) == -1)
                {
                   perror("Cannot create loader");
                   exit(-1);
                }
            }
        }
    }

    for(i = 0; i < workerNumber; i++) waitpid(pid[i],&status,0);
}
