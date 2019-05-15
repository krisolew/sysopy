#include <stdlib.h>
#include <mqueue.h>
#include "settings.h"

int main()
{
    if (mq_unlink(SERVER_NAME) == -1){
        perror("Cannot remove server queue");
        exit(-1);
    }

    return 0;
}