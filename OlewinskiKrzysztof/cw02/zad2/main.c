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

char * operator;
time_t t;

bool compare_times(time_t file_date)
{
    switch(operator[0]){
        case '>': 
            return file_date > t;
        case '=': 
            return file_date = t;
        case '<': 
            return file_date < t;
    }
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
        printf("rozmiar: %d bajtow\n", status->st_size);

        struct tm *tm;
        char buf[20];
        time_t t;

        //ostatnii dostep
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

int main(int argc, char* argv[])
{
    char * director = argv[1];
    operator = argv[2];
    char * date = argv[3];

    struct tm tm={0,0,0,0,0,0,0,0,0};
    strptime(date, "%d.%m.%Y", &tm);
    t = mktime(&tm);

    search(director);

    return 0;
}