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

void send_request(enum Command_t type, char request[MAX_MESSAGE_LENGTH])
{
    struct Message_t message;
    message.type = type;
    strcpy(message.content, request);
    message.senderId = clientID;

    if (msgsnd(serverQueueID, &message, MSGSZ, IPC_NOWAIT) == -1)
    {
        perror("Cannot send request to server");
        return;
    }
}

void receive_response(struct Message_t *message)
{
    if (msgrcv(clientQueueID, message, MSGSZ, -(NUMBER_OF_COMMANDS + 1), 0) == -1)
    {
        perror("Cannot receive server response");
    }
}

void exec_init()
{
    struct Message_t message;
    char request[MAX_MESSAGE_LENGTH];

    sprintf(request, "%i", clientQueueID);

    message.type = INIT;
    strcpy(message.content, request);
    message.senderId = getpid();

    if (msgsnd(serverQueueID, &message, MSGSZ, IPC_NOWAIT) == -1)
    {
        perror("Cannot send request to server");
        return;
    }

    receive_response(&message);

    if (message.type != INIT)
    {
        perror("Wrong type of response");
        return;
    }
    sscanf(message.content, "%i", &clientID);
}

void exec_stop()
{
    stop = 1;
    send_request(STOP, "");
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

    exec_init();

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