#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t child_pid;
    printf("PID glownego programu: %d\n", (int)getpid());
    child_pid = fork();
    if (child_pid==0)
    {
        printf("PID dziecka: %d\n", (int)child_pid);
        exit(0);
    }
    else
    {
        sleep(10);
        exit(0);
    }
}
