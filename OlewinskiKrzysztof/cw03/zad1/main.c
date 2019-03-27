#define _XOPEN_SOURCE 500
#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<time.h>
#include<stdbool.h>
#include<stdlib.h>
#include<ftw.h>
#include<dirent.h>
#include<string.h>
#include<ctype.h>
#include<errno.h>

char * build_file_path(struct dirent *file, char * path)
{
    char * result = malloc ( 200 * sizeof(char*) );
    strcpy(result, path);
    strcat(result, "/");
    strcat(result, file->d_name);
    strcat(result, "\0");

    return result;
}

void search(char *path)
{
    DIR * stream = opendir(path);

    struct dirent *file;

    while((file = readdir(stream)) != NULL){
        if(file->d_name[0] == '.' && (file->d_name[1]=='\0' || file->d_name[1]=='.')) continue;

        struct stat status;
        char *full_path = build_file_path(file,path);

        if(lstat(full_path, &status)==-1){
            perror("Something went wrong with lstat: " );
            errno = 0;
        }

        if (S_ISDIR(status.st_mode))
        {
            char *command = malloc(300 * sizeof(char));
            strcpy(command, "ls -l ");
            strcat(command, full_path);
            if (vfork() == 0){
                printf("PID %d \t %s\n", getpid(), path);
                system(command);
                search(full_path);
            } 
            
            free(command);
        }
        free(full_path);
    }

    if(errno != 0){
        perror("Something went wrong with readdir: " );
    }

    if(closedir(stream)==-1){
        perror("Something went wrong when closing a stream: " );
    }
    exit(0);
}

int main(int argc, char* argv[])
{
    char * actualpath= malloc(200 * sizeof(char));
    realpath(argv[1], actualpath);
    search(actualpath);

    return 0;
}
