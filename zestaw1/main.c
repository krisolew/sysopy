#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"library.h"
#include<string.h>
#include<time.h>
#include<sys/times.h>

int main(int argc, char* argv[]){

    int opt;
    int index = 1, size, current_index=0, delete_index;
    char *dir, *file, *tmp_file;
    char **table;
    int i=0;

    struct timespec start, stop;
    double duration;
    static struct tms st_cpu;
    static struct tms en_cpu;

    clock_gettime(CLOCK_REALTIME, &start);
    times(&st_cpu);

    int liczba_opcji = 0;
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
                
                table = create_table(size);
                break;
            case 'f':
        
                dir = strdup(argv[index++]);
                file = strdup(argv[index++]);
                tmp_file = strdup(argv[index]);

                
                find(dir, file, tmp_file);

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

                delete(delete_index, table, size);
                break;
        }
        index++;
        liczba_opcji++;
    }

    times(&en_cpu);
    clock_gettime(CLOCK_REALTIME, &stop);

    duration = stop.tv_sec - start.tv_sec + (stop.tv_nsec - start.tv_nsec) * 1.0/1000000000;
    printf ("Real time %lf s\n", duration);
    printf ("Cpu user time %Lf s\n", (long double)(en_cpu.tms_utime - st_cpu.tms_utime)/sysconf(_SC_CLK_TCK));
    printf ("Cpu system time %Lf s\n", (long double)(en_cpu.tms_stime - st_cpu.tms_stime)/sysconf(_SC_CLK_TCK));
    printf ("Liczba przekazanych opcji: %d\n", liczba_opcji);
    
    return 0;
}
