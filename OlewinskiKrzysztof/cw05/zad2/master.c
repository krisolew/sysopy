#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("Expected one argument - file name\n");
        return -1;
    }
    
    if( mkfifo(argv[1], S_IRUSR | S_IWUSR) < 0)
    {
        printf("Error when creating fifo\n");
        return -1;
    }

    int desc = open(argv[1],O_RDONLY);

    if(desc == -1)
    {
        printf("Error when opening fifo\n");
        return -1;
    }

    char *buff = malloc(1024*sizeof(char));

    while(1)
    {
        if(read(desc, buff,1024) != 0)
        {
            printf("%s",buff);
        }

    }
    return 0;
}