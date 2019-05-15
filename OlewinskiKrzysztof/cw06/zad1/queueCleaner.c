#include <sys/msg.h>
#include <stdlib.h>
#include "settings.h"

int main()
{
    int queueID = -1;

    if ((queueID = msgget(getServerQueueKey(), 0)) == -1)
    {
        perror("Cannot create server queue");
        return -1;
    }

    if (msgctl(queueID, IPC_RMID, NULL) == -1){
        perror("Cannot remove server queue");
        return -1;
    }

    return 0;
}
