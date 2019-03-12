#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>

char **create_table(char size){
    return malloc(size * sizeof(char *));
}

void find(const char * dir_name, const char * file_name, const char * tmp){
    char * command = malloc(50 * sizeof(char));

    sprintf(command,"find \"%s\" -name \"%s\" > \"%s\"",dir_name, file_name, tmp);
    system(command);
}

void save(char** table, int current_index)
{
    FILE* fp = fopen("tmp.txt", "r");
    fseek(fp, 0L, SEEK_END);
    int res = ftell(fp);
    fclose(fp);
    
    char *pointer = calloc( (size_t) (res + 1), sizeof(char));

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

int main(){

    find("katalog", "tekst2.txt", "tmp.txt");
    char ** table = create_table(20);
    int current_index=0;
    save(table, current_index);

    printf("%c", table[0][2]);
    return 0;
}