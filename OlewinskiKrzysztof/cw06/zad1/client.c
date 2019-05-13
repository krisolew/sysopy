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

int serverQueueID = -1;
int clientQueueID = -1;
int clientID = -1;
int stop = 0;

void exec_init(char args[MAX_MESSAGE_LENGTH])
{

}

void exec_stop()
{
    
}

void exec_echo(char args[MAX_MESSAGE_LENGTH])
{
    
}

void exec_list()
{
    
}

void exec_friends(char args[MAX_MESSAGE_LENGTH])
{
    
}

void exec_2one(char args[MAX_MESSAGE_LENGTH])
{
    
}

void exec_2friends(char args[MAX_MESSAGE_LENGTH])
{
    
}

void exec_2all(char args[MAX_MESSAGE_LENGTH])
{
    
}

void exec_add(char args[MAX_MESSAGE_LENGTH])
{
    
}

void exec_del(char args[MAX_MESSAGE_LENGTH])
{
    
}

void exec_read(char args[MAX_MESSAGE_LENGTH])
{
    
}

int execute_command(FILE *file)
{
    char args[MAX_MESSAGE_LENGTH], command[MAX_MESSAGE_LENGTH];
    if (fgets(args, MAX_MESSAGE_LENGTH * sizeof(char), file) == NULL) return EOF;
    int numberOfArguments = sscanf(args, "%s", command);
    if (numberOfArguments == EOF || numberOfArguments == 0) return 0;

    if (strcmp(command, "ECHO") == 0) {
        exec_echo(args);
    } else if (strcmp(command, "LIST") == 0) {
        exec_list();
    } else if (strcmp(command, "FRIENDS") == 0) {
        exec_friends(args);
    } else if (strcmp(command, "2ALL") == 0) {
        exec_2all(args);
    } else if (strcmp(command, "2FRIENDS") == 0) {
        exec_2friends(args);
    } else if (strcmp(command, "2ONE") == 0) {
        exec_2one(args);
    } else if (strcmp(command, "STOP") == 0) {
        exec_stop();
        return EOF;
    } else if (strcmp(command, "READ") == 0) {
        exec_read(args);
    } else if (strcmp(command, "ADD") == 0) {
        exec_add(args);
    } else if (strcmp(command, "DEL") == 0) {
        exec_del(args);
    } else {
        printf("Unknown command\n");
    }

    return 0;
}

void finishWork(int signalno)
{
    exec_stop();
    stop = 1;
}

int main()
{
    signal(SIGINT, finishWork);

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

    while(!stop)
    {
        
    }

    if (msgctl(clientQueueID, IPC_RMID, NULL) == -1)
    {
        perror("Cannot remove client queue");
        return -1;
    }
    
    return 0;
}