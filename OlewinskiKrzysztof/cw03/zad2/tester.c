#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    srand(time(NULL));
    if (argc != 5)
    {
        printf("Expected 4 arguments\n");
        return -1;
    }

    char *file_name = argv[1];

    int pmin;
    if(sscanf(argv[2],"%d", &pmin) != 1)
    {
        printf("Second parametr should be integer\n");
        return -1;
    } 

    int pmax;
    if(sscanf(argv[3],"%d", &pmax) != 1)
    {
        printf("Third parametr should be integer\n");
        return -1;
    }

    int bytes;
    if(sscanf(argv[4],"%d", &bytes) != 1)
    {
        printf("Fourth parametr should be integer\n");
        return -1;
    }

    int freq = rand() % (pmax - pmin) + pmin;
    char * real_path = malloc (300 * sizeof(char));
    realpath(file_name,real_path);

    FILE *fp;
    fp = fopen(real_path, "a");
    struct tm *t;
    char dest[257];
    char seconds[128];
    char timestring[128];
    char pid[15];

    while(1>0)
    {
        sleep(freq);
    
        time_t mytime = time((time_t*)0);
        t = localtime(&mytime);
        strftime(timestring, 128, "%c", t);
        sprintf(seconds, "%d", freq);
        sprintf(pid, "%d", getpid());
        strcpy(dest, pid);
        strcat(dest, " ");
        strcat(dest, seconds);
        strcat(dest, " ");
        strcat(dest, timestring);
        strcat(dest, " ");
        fseek(fp, 0, SEEK_END);
        fputs(dest, fp);
        int i = 0;
        while(i < bytes){
            fputs("A",fp);
            i++;
        }
        fputs("\n",fp);
    }

    return 0;
}
