#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h> 
#include <libgen.h>
#include <time.h>
#include <signal.h>

int END = 0;
int STOP = 0;
pid_t pid[1024];
char *names[1024];
int count = 0;

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

    chdir("./archiwum");
    FILE * fp = fopen(new_file_name, "w+"); 
    fwrite(copy_tab, last_size, sizeof(char), fp);

    fclose(fp);
    free(new_file_name);
}

void exec_cp(char *file_name, time_t last_modification)
{
    struct tm *tm;
    char buf[40];

    tm = localtime(&last_modification);
    strftime(buf, sizeof(buf), "_%Y-%m-%d_%H-%M-%S", tm);

    char *new_file_name = malloc (100 * sizeof(char));
    strcpy(new_file_name, "./archiwum/");
    strcat(new_file_name, basename(file_name));
    strcat(new_file_name, buf);
    
    execlp("cp", "cp", file_name, new_file_name, NULL);

    free(new_file_name);
}

void memory(FILE * file, struct stat status, char * file_name, int seconds)
{
    if(stat(file_name, &status) != 0){
        printf("problem with file\n");
        return;
    }

    time_t last_modification = status.st_mtime;
    int last_size = status.st_size;
    char * copy_tab = copy_to_memory(file_name);

    int num_of_copies = 0;

    while (END == 0)
    {
        if (STOP == 0)
        {
            sleep(seconds);
            stat(file_name, &status);
            if (status.st_mtime != last_modification)
            {
                num_of_copies++;
                write_to_arch(copy_tab,file_name,last_modification, last_size);
                last_modification = status.st_mtime;
                last_size = status.st_size;
                free(copy_tab);
                copy_tab = copy_to_memory(file_name);
            }
        }
    }

    free(copy_tab);
    exit(num_of_copies);
}


void disc(FILE * file, struct stat status, char * file_name, int seconds)
{
    if(stat(file_name, &status) != 0){
        printf("problem with file\n");
        return;
    }

    time_t last_modification = status.st_mtime;

    int num_of_copies = 0;

    while (END == 0)
    {
        if (STOP == 0)
        {
            sleep(seconds);
            stat(file_name, &status);
            if (status.st_mtime != last_modification)
            {
                num_of_copies++;
                if (vfork() == 0)
                {
                    exec_cp(file_name, last_modification);
                }
                last_modification = status.st_mtime;
            }
        }
    }
    exit(num_of_copies);
}

void print_list()
{
    int i;
    for (i=0; i<count; i++)
    {
        int status;
        printf("Proces %d monitoruje plik %s\n", pid[i], names[i]);
    }
}

void print_raport(){ 

    int i; 
    for(i=0; i<count; i++) kill(pid[i], SIGQUIT);
    for (i=0; i<count; i++)
    {
        int status;
        waitpid(pid[i], &status, 0);
        printf("Proces %d utworzyl %d kopi pliku %s\n", pid[i], (status/256), names[i]);
    }
}

void signal_usr1(int sig_num)
{
    STOP = 1;
}

void signal_usr2(int sig_num)
{
    STOP = 0;
}

void signal_quit(int sig_num)
{
    END = 1;
}

void signal_int(int sig_num)
{
    print_raport();
    exit(0);
}



int main(int argc, char *argv[])
{
    signal(SIGUSR1, signal_usr1);
    signal(SIGUSR2, signal_usr2);
    signal(SIGINT, signal_int);


    if (argc != 3)
    {
        printf("Expected 2 arguments\n");
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
    struct stat status;

    int j;
    for(j=0; j<1024; j++){
        names[j] = malloc(150 * sizeof(char));
    }

    char * real_path = malloc (300 * sizeof(char));

    char *new_dir_name = malloc (100 * sizeof(char));
    strcpy(new_dir_name, "./archiwum");
    mkdir(new_dir_name,  S_IRWXU);
    free(new_dir_name);

    while(fscanf(file,"%s %d", file_name, &seconds) == 2) {
        
        realpath(file_name,real_path);
        if(strcmp(argv[2], "memory") == 0) {
            strcpy(names[count],file_name);
            pid[count] = fork();    
            if(pid[count] == 0 ) {
                signal(SIGQUIT, signal_quit);
                memory(file, status, real_path, seconds);
            }
        }
        else if(strcmp(argv[2], "disc") == 0) {
            printf("disc %s\n", file_name);
            strcpy(names[count],file_name);
            pid[count] = fork();
            if(pid[count] == 0 ) {
                signal(SIGQUIT, signal_quit);
                disc(file, status, real_path, seconds);
            }
        }
        else {
            printf("Unsupported operation. Closing program!\n");
            free(file_name); 
            fclose(file);
            return -1;
        }
        count = count+1;
    }

    print_list(names, pid, count);

    char *command = malloc (20 * sizeof(char));

    while(1)
    {
        scanf("%s", command);

        if ( strcmp(command,"LIST") == 0 )
        {
            print_list();
        }

        else if ( strcmp(command,"START_ALL") == 0 )
        {
            kill(0, SIGUSR2);  
        }

        else if ( strncmp(command,"START", 5) == 0 )
        {
            int pid1;
            scanf("%d", &pid1);
            kill(pid1, SIGUSR2);
        }

        else if ( strcmp(command,"STOP_ALL") == 0 )
        {
            kill(0, SIGUSR1);               
        }
        
        else if ( strncmp(command,"STOP", 4) == 0 )
        {
            int pid1;
            scanf("%d", &pid1);
            kill(pid1, SIGUSR1);
        }

        else if ( strcmp(command,"END") == 0 )
        {
            print_raport();
            break;
        } 
    }  
    return 0;
}