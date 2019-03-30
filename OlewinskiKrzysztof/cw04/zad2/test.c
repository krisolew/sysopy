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

int main(){
    char *names[1024];

    names[0] = "Anka";`
    names[1] = "Oliwka";

    printf(names[0]);
    printf(names[1]);

    return 0;
}