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
int last_clientID = -1;
int queueID = -1;

void send_response(int clientID, enum Command_t type, char msg[MAX_MESSAGE_LENGTH]);

void exec_stop(pid_t senderId)
{
}

void exec_echo(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
}

void exec_init(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    last_clientID++;
    if (last_clientID > MAX_NUMBER_OF_CLIENTS)
    {
        perror("To mamy clients on server");
        return;
    }

    int clientQueueId;
    sscanf(msgContent, "%i", &clientQueueId);

    clients[last_clientID].pid = senderId;
    clients[last_clientID].current_friends_number = 0;
    clients[last_clientID].queueID = clientQueueId;

    char message[MAX_MESSAGE_LENGTH];
    sprintf(message, "%i", last_clientID);
    send_response(last_clientID, INIT, message);
}

void exec_list(pid_t senderId)
{
}

void exec_friends(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
}

void exec_2all(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
}

void exec_2friends(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
}

void exec_2one(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
}

void exec_add(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
}

void exec_del(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
}

void send_response(int clientID, enum Command_t type, char msg[MAX_MESSAGE_LENGTH])
{
    struct Message_t message;
    message.msgType = type;
    strcpy(message.msgContent, msg);
    message.senderId = -1;

    if (msgsnd(clients[clientID].queueID, &msg, MSGSZ, IPC_NOWAIT) == -1) perror("Cannot send response to client");
}

void handle_message(struct Message_t *message) {
    switch (message->msgType) {
        case STOP:
            exec_stop(message->senderId);
            break;
        case ECHO:
            exec_echo(message->senderId, message->msgContent);
            break;
        case INIT:
            exec_init(message->senderId, message->msgContent);
            break;
        case LIST:
            exec_list(message->senderId);
            break;
        case FRIENDS:
            exec_friends(message->senderId, message->msgContent);
            break;
        case _2ALL:
            exec_2all(message->senderId, message->msgContent);
            break;
        case _2FRIENDS:
            exec_2friends(message->senderId, message->msgContent);
            break;
        case _2ONE:
            exec_2one(message->senderId, message->msgContent);
            break;
        case ADD:
            exec_add(message->senderId, message->msgContent);
            break;
        case DEL:
            exec_del(message->senderId, message->msgContent);
            break;
    }
}

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

    struct Message_t message;
    while(1)
    {
        if (msgrcv(queueID, &message, MSGSZ, -(NUMBER_OF_COMMANDS + 1), 0) == -1)
        {
            perror("Cannot receive message form queue");
            return -1;
        }
        handle_message(&message);
    }

    msgctl(queueID, IPC_RMID, NULL);

    return 0;
}