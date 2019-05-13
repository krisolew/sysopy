#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "settings.h"

int serverQueueID;
int clientQueueID;
int stop = 0;

int main()
{
    if ((serverQueueID = msgget(getServerQueueKey(), 0)) == -1)
    {
        perror("Cannot create server queue");
        return -1;
    }

    if ((clientQueueID = msgget(getClientQueueKey(), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {
        perror("Cannot create client queue");
        return -1;
    }
    
    return 0;
}