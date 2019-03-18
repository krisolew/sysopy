#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"library.h"
#include<string.h>
#include<time.h>
#include<sys/times.h>
#include <dlfcn.h>
#define PLIK "results3a.txt"



struct timespec start, stop;
static struct tms st_cpu;
static struct tms en_cpu;

void measure_start_time()
{
    clock_gettime(CLOCK_REALTIME, &start);
    times(&st_cpu);
}

void measure_stop_time()
{
    times(&en_cpu);
    clock_gettime(CLOCK_REALTIME, &stop);
}

void print_and_save_times(char parametr)
{
    FILE* file = fopen(PLIK,"a+");

    switch(parametr){
        case 'f':
            printf("wywołanie funkcji find: \n");
            fprintf(file, "wywołanie funkcji find: \n");
            break;
        case 'd':
            printf("wywołanie funkcji delete: \n");
            fprintf(file, "wywołanie funkcji delete: \n");
            break;
        case 'c':
            printf("wywołanie funkcji create_table: \n");
            fprintf(file, "wywołanie funkcji create_table: \n");
            break;
    }

    double duration = stop.tv_sec - start.tv_sec + (stop.tv_nsec - start.tv_nsec) * 1.0/1000000000;
    long double cpu_user_time = (long double)(en_cpu.tms_utime - st_cpu.tms_utime)/sysconf(_SC_CLK_TCK);
    long double cpu_system_time = (long double)(en_cpu.tms_stime - st_cpu.tms_stime)/sysconf(_SC_CLK_TCK);

    printf("Real time %lf s\n", duration);
    printf("Cpu user time %Lf s\n", cpu_user_time);
    printf("Cpu system time %Lf s\n", cpu_system_time);
    printf("\n");

    fprintf(file, "Real time %lf s\n", duration);
    fprintf(file, "Cpu user time %Lf s\n", cpu_user_time);
    fprintf(file, "Cpu system time %Lf s\n", cpu_system_time);
    fprintf(file, "\n");
    fclose(file);
}


int main(int argc, char* argv[]){

#ifdef DLL
    void *handle = dlopen("./liblibrary.so", RTLD_LAZY);

    char ** (*create_table)(int) = dlsym(handle,"create_table");
    void (*find)(const char *, const char * , const char *)= dlsym(handle,"find");
    void (*save)(char**, int) = dlsym(handle,"save");
    void (*delete1)(int, char **, int) = dlsym(handle,"delete1");
#endif

    int opt;
    int index = 1, size=0, current_index=0, delete_index=0;
    char *dir, *file, *tmp_file;
    char **table=create_table(size);
    int i=0;

    while ((opt=getopt(argc,argv,"c:f:d:")) != -1)
    {
        index++;
        switch(opt)
        {
            case 'c':
                size = 0;
                i = 0;
                while (optarg[i]!='\0')
                {
                    size*=10;
                    size += (int) optarg[i] - '0';
                    i++;
                }

                measure_start_time();
                table = create_table(size);
                measure_stop_time();

                FILE* file1 = fopen(PLIK,"a");
                fprintf(file1, "polecenie: ");
                for(int i = 1; i < 3; i++){
                    fprintf(file1, " %s ", argv[i]);
                }
                fprintf(file1, "\n");
                fclose(file1);

                print_and_save_times(opt);
                break;

            case 'f':

                dir = strdup(argv[index++]);
                file = strdup(argv[index++]);
                tmp_file = strdup(argv[index]);

                file1 = fopen(PLIK,"a");
                fprintf(file1, "polecenie: ");
                fprintf(file1, " -f %s %s %s ", dir, file, tmp_file);
                fprintf(file1, "\n");
                fclose(file1);

                measure_start_time();
                find(dir, file, tmp_file);
                measure_stop_time();
                print_and_save_times(opt);

                save(table,current_index);
                break;

            case 'd':
                delete_index = 0;
                i = 0;
                while (optarg[i]!='\0')
                {
                    delete_index*=10;
                    delete_index += (int) optarg[i] - '0';
                    i++;
                }

                file1 = fopen(PLIK,"a");
                fprintf(file1, "polecenie: ");
                fprintf(file1, " -d %d", delete_index);
                fprintf(file1, "\n");
                fclose(file1);

                measure_start_time();
                delete1(delete_index, table, size);
                measure_stop_time();
                print_and_save_times(opt);
                break;
        }
        index++;
    }
#ifdef DLL
    dlclose(handle);
#endif
    return 0;

}


