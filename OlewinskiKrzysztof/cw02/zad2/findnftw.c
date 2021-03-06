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

char * operator;
time_t t;

bool compare_times(time_t file_date)
{
    switch(operator[0]){
        case '>': 
            return (file_date > t);
        case '=': 
            return (file_date = t);
        case '<': 
            return (file_date < t);
    }
    return true;
}

int file_info( const char * file, const struct stat *status, int flag, struct FTW * s)
{
    if (compare_times(status->st_mtime)){

        //sciezka
        printf("%s\n",file);
        
        //typ
        if (S_ISLNK(status->st_mode)) printf("slink\n");
        else if (S_ISREG(status->st_mode)) printf("file\n");
        else if (S_ISDIR(status->st_mode)) printf("dir\n");
        else if (S_ISBLK(status->st_mode)) printf("block dev\n");
        else if (S_ISCHR(status->st_mode)) printf("char dev\n");
        else if (S_ISFIFO(status->st_mode)) printf("fifo\n");
        else if (S_ISSOCK(status->st_mode)) printf("sock\n");

        //rozmiar
        printf("rozmiar: %ld bajtow\n", status->st_size);

        struct tm *tm;
        char buf[20];
        time_t t;

        //ostatni dostep
        t = status->st_atime;
        tm = localtime(&t);
        strftime(buf, sizeof(buf), "%d.%m.%Y ", tm);
        printf("dostep: %s\n", buf);

        //ostatnia modyfikacja
        t = status->st_mtime;
        tm = localtime(&t);
        strftime(buf, sizeof(buf), "%d.%m.%Y ", tm);
        printf("modyfikacja: %s\n\n", buf);
    }
    return 0;
}

void search(char *path){
    int flag = FTW_PHYS;
    printf("%d",nftw(path, file_info, 2000, flag));
}

bool is_operator_correct()
{
    char ch;
    int i = 0;
    while ((ch = operator[i]) != '\0')
    {
        i++;
    }

    if ( i != 1 ) 
    {
        printf("incorrect operator format! You have to enter only one character\n");
        return false;
    }

    if (operator[0] != '=' && operator[0] != '>' && operator[0] != '<')
    {
        printf("incorrect operator! List of availible operators : '=', '>', '<' \n");
        return false;
    }

    return true;
}

bool is_date_correct(char *date)
{
    char ch;
    int i = 0;
    while ((ch = date[i]) != '\0')
    {
        i++;
    }

    if ( i != 10 ) 
    {
        printf("incorrect date length! You have to enter 10 characters\n");
        return false;
    }

    if ( date[2] != '.' || date[5] != '.')
     {
        printf("incorrect date format! Example of correct format of date 'dd.mm.yyyy' \n");
        return false;
    }

    if( !(isdigit(date[0]) && isdigit(date[1]) && isdigit(date[3]) && isdigit(date[4]) && isdigit(date[6]) && isdigit(date[7]) && isdigit(date[8]) && isdigit(date[9])))
    {
        printf("incorrect characters! List of correct characters: '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' \n");
        return false;
    }

    return true;
}

bool is_number_of_agrs_correct(int argc)
{
    if ( argc != 4 ) 
    {
        printf("incorrect number of arguments! You have to enter 4 arguments\n");
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    if (!is_number_of_agrs_correct(argc)) return -1;
    char * director = argv[1];
    
    operator = argv[2];
    if (!is_operator_correct()) return -2;
    
    char * date = argv[3];
    if (!is_date_correct(date)) return -3;

    struct tm tm={0,0,0,0,0,0,0,0,0};
    strptime(date, "%d.%m.%Y", &tm);
    t = mktime(&tm);

    search(director);

    return 0;
}
