#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char* commands[100][100];

int count_commands(char *buffer){
    char* p = strtok(buffer," ");
    int i = 0;
    int j = 0;
    while(p != NULL){
        if(p[0] == '|' && p[1] == '\0'){
            commands[i][j] = NULL;
            i++;
            j = 0;
        }
        else{
            commands[i][j++] = p;
        }
        p = strtok(NULL," \n");
    }
    commands[i][j] = NULL;
    return i;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Expected one argument - file name\n");
        return -1;
    }

    FILE *file;
    file = fopen(argv[1], "r");

    if ( file == NULL ){
        printf("Cannot open file");
        return -1;
    }

    char *buf = malloc( 1024 * sizeof(char) );
    int arg_n;
    int i;
    int curr[2];
    int prev[2];
    int child_pids[256];
    int status;

    while(1)
    {
        if (fgets(buf, 1024 * sizeof(char), file) == NULL) break;

        arg_n = count_commands(buf);
        for(i = 0; i <= arg_n; i++){
            if(pipe(curr) == -1)
            {
                printf("Cannot create pipe\n");
                exit(-1);
            }
            child_pids[i] = fork();
            if(child_pids[i] == 0){
                if(i > 0){
                    dup2(prev[0],STDIN_FILENO);
                    close(prev[1]);
                }
                if(i < arg_n){
                    dup2(curr[1],STDOUT_FILENO);
                    close(curr[0]);
                }
                execvp(commands[i][0],commands[i]);
                exit(0);
            }
            else if(child_pids[i] > 0){
                if(i > 0){
                    close(prev[0]);
                    close(prev[1]);
                }
                if(i < arg_n){
                    prev[0] = curr[0];
                    prev[1] = curr[1];
                }
            }
        }
        for(i = 0; i < arg_n; i++) waitpid(child_pids[i],&status,0);
    }

    return 0;
}