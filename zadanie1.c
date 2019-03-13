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
    char ** table;

    while ((opt=getopt(argc,argv,"c:f:d:")) != -1)
    {
        index++;
        switch(opt)
        {
            case 'c':
                size = (int) optarg[0] - '0';
                table = create_table(size);
                break;
            case 'f':
        
                dir = strdup(argv[index++]);
                file = strdup(argv[index++]);
                tmp_file = strdup(argv[index]);

                find(dir, file, tmp_file);
                save(table, current_index);
                break;
            case 'd':
                delete_index = (int) optarg[0] - '0';
                delete(delete_index, table, size);
                break;
        }
        index++;
    }

    // char ch;
    // int i = 0;
    // while ( (ch = dir[i]) != '\0' ){
    //     printf("%c", ch);
    //     i++;
    // }
    // printf("\n");
    // i = 0;
    // while ( (ch = file[i]) != '\0' ){
    //     printf("%c", ch);
    //     i++;
    // }
    //     printf("\n");
    // i = 0;
    // while ( (ch = tmp_file[i]) != '\0' ){
    //     printf("%c", ch);
    //     i++;
    // }    printf("\n");
    
    return 0;
}
