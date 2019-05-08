#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>

#include "settings.h"

struct Client_t
{
    int queueID;
    int friends[MAX_NUMBER_OF_CLIENTS];
    int current_friends_number;
    pid_t pid;
};

struct Client_t clients[MAX_NUMBER_OF_CLIENTS];
int queueID = -1;

int main()
{
    int i;
    for (i=0; i < MAX_NUMBER_OF_CLIENTS; i++)
    {
        clients[i].queueID = -1;
        clients[i].current_friends_number = 0;
    }

    queueID = msgget(getServerQueueKey(), IPC_CREAT | IPC_EXCL | 0666);
    if (queueID == -1)
    {
        perror("Cannot create server queue");
        return -1;
    }

    msgctl(queueID, IPC_RMID, NULL);

    return 0;
}