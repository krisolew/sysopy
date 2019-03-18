#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>

char **create_table(int size)
{
    return calloc( size, sizeof(char *));
}

void find(const char * dir_name, const char * file_name, const char * tmp)
{
    char * command = calloc(50, sizeof(char));

    sprintf(command,"find \"%s\" -name \"%s\" > \"%s\"",dir_name, file_name, tmp);
    if(system(command)!=1) return;
}

void save(char** table, int current_index)
{
    FILE* fp = fopen("tmp.txt", "r");
    fseek(fp, 0L, SEEK_END);
    int res = ftell(fp);
    fclose(fp);

    char *pointer = calloc( (res + 1), sizeof(char));

    fp = fopen("tmp.txt", "r");
    char ch;
    int i=0;
    while((ch = fgetc(fp)) != EOF )
    {
        pointer[i]=ch;
        i++;
    }

    table[current_index] = pointer;
}

void delete1(int index, char ** table, int size)
{
    if ( index >= 0 && index < size ) free(table[index]);
}