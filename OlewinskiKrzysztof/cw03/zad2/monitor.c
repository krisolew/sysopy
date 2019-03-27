#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h> 
#include <libgen.h>
#include <time.h>

char* copy_to_memory(char *file_name)
{
    FILE* fp = fopen(file_name, "r");
    fseek(fp, 0, SEEK_END);
    int res = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char * buf = calloc((size_t) res, sizeof(char));
    fread(buf, sizeof(char), res, fp);
    fclose(fp);
    return buf;
}

void write_to_arch(char *copy_tab, char * file_name, time_t last_modification, int last_size)
{
    struct tm *tm;
    char buf[40];

    tm = localtime(&last_modification);
    strftime(buf, sizeof(buf), "_%Y-%m-%d_%H-%M-%S", tm);

    char *new_file_name = malloc (100 * sizeof(char));
    strcpy(new_file_name, basename(file_name));
    strcat(new_file_name, buf);

    char *new_dir_name = malloc (100 * sizeof(char));
    strcpy(new_dir_name, "./archiwum");
    
    mkdir(new_dir_name,  S_IRWXU);
    chdir(new_dir_name);
    FILE * fp = fopen(new_file_name, "w+"); 
    fwrite(copy_tab, last_size, sizeof(char), fp);
    fclose(fp);
}

void memory(FILE * file, struct stat status, char * file_name, int seconds, int time_to_stop)
{
    if(stat(file_name, &status) != 0){
        printf("problem with file\n");
        return;
    }

    time_t last_modification = status.st_mtime;
    int last_size = status.st_size;
    char * copy_tab = copy_to_memory(file_name);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    gettimeofday(&end, NULL);

    while ((end.tv_sec - start.tv_sec) <= time_to_stop)
    {
        sleep(seconds);
        stat(file_name, &status);
        if (status.st_mtime != last_modification)
        {
            write_to_arch(copy_tab,file_name,last_modification, last_size);
            last_modification = status.st_mtime;
            last_size = status.st_size;
            free(copy_tab);
            copy_tab = copy_to_memory(file_name);
        }
        gettimeofday(&end, NULL);
    }

    return;
}   

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Expected 3 arguments\n");
        return -1;
    }

    FILE *file;
    file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("Problems with opening file\n");
    }

    char *file_name = malloc( 300 * sizeof(char));
    int seconds;
    int time_to_stop;
    int count = 0;
    struct stat status;

    if(sscanf(argv[2],"%d", &time_to_stop) != 1)
    {
        printf("Second parametr should be integer\n");
        return -1;
    }

    pid_t pid[1024];
    char * real_path = malloc (300 * sizeof(char));
    
    while(fscanf(file,"%s %d", file_name, &seconds) == 2) {
        
        realpath(file_name,real_path);
        if(strcmp(argv[3], "memory") == 0) {
            pid[count] = fork();
            if(pid[count] == 0 ) {    
                memory(file, status, real_path, seconds, time_to_stop);
            }
        // }
        // else if(strcmp(argv[3], "disc") == 0) {
        //     pid[count] = fork();
        //     if(pid[count] == 0 ) {
        //         child_process_on_disc(file, status, file_name, seconds, time_to_stop);
        //     }
        // }
        // else {
        //     printf("Unsupported operation. Closing program!\n");
        //     free(file_name); 
        //     fclose(file);
        //     return -1;
        }
        count = count+1;
    }
    sleep(time_to_stop);
}