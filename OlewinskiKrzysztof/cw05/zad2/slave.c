#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


int main(int argc, char **argv)
{
    srand(time(NULL));

    if(argc != 3)
    {
        printf("Expected two arguments - file name and natural number\n");
        return -1;
    }

    int desc = open(argv[1],O_WRONLY);
    char buffer[128];
    char date_str[128];
    FILE* pipe;


    if(desc == -1)
    {
        printf("Error when opening fifo\n");
        return -1;
    }

    int n;
    sscanf(argv[2],"%d",&n);
  
    printf("%d \n", getpid());

    while(n--)
    {
        pipe = popen("date","r");

        if(pipe == NULL)
        {
            printf("Error when opening fifo date\n");
            break;
        }

        sprintf(buffer, "%d ", getpid());
        fread(date_str, sizeof(char), 128, pipe);
        strcat(buffer, date_str);
        write(desc,buffer, 128*sizeof(char));

        if(pclose(pipe) != 0)
        {
            printf("Error when closing fifo date\n");
            break;
        }

        sleep(rand() % 4 + 2);
    }

    if(close(desc) != 0)
    {
        printf("Error when closing fifo\n");
        return -1;
    }
    return 0;
}