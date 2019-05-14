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
int stop = 0;

void send_response(int clientID, enum Command_t type, char response[MAX_MESSAGE_LENGTH]);

void add_friends(int clientID, char list_of_friends[MAX_MESSAGE_LENGTH])
{
    char *friend = strtok(list_of_friends, "|");

    while (friend != NULL && clients[clientID].current_friends_number < MAX_NUMBER_OF_CLIENTS)
    {
        int friendID = strtol(friend, NULL, 10);
        int exists = 0, i = 0;

        for (; i < clients[clientID].current_friends_number; i++)
        {
            if (friendID == clients[clientID].friends[i]) exists = 1;
        }

        if (!exists)
        {
            clients[clientID].friends[clients[clientID].current_friends_number] = friendID;
            clients[clientID].current_friends_number++;
        }

        friend = strtok(NULL, "|");
    }
}

void del_friends(int clientID, char list_of_friends[MAX_MESSAGE_LENGTH])
{
    char *friend = strtok(list_of_friends, "|");

    while (friend != NULL && clients[clientID].current_friends_number > 0)
    {
        int friendID = strtol(friend, NULL, 10);
        int exists = 0, i = 0;

        for (; i < clients[clientID].current_friends_number; i++)
        {
            if (friendID == clients[clientID].friends[i]) {
                exists = 1;
                break;
            }
        }

        if (exists && i < clients[clientID].current_friends_number)
        {
            clients[clientID].friends[friendID] = clients[clientID].friends[--clients[clientID].current_friends_number];
        }

        friend = strtok(NULL, "|");
    }
}

void exec_stop(int senderId)
{
    printf("Received stop request from client %d", senderId);

    if ( senderId >=0 && senderId < MAX_NUMBER_OF_CLIENTS)
    {
        clients[senderId].queueID = -1;
        clients[senderId].current_friends_number = 0;
        for (int i = 0; i < MAX_NUMBER_OF_CLIENTS; i++)
            clients[senderId].friends[i] = -1;
    }
    else
    {
        perror("Cannot delet client from server");
    }
}

void exec_echo(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received echo request from client %d", senderId);

    char response[MAX_MESSAGE_LENGTH];
    char date[31];
    FILE *f = popen("date", "r");
    fread(date, sizeof(char), 31, f);
    pclose(f);
    sprintf(response, "%s Date: %s", msgContent, date);
    send_response(senderId, ECHO, response);
}

void exec_init(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received init request from client %d", senderId);
    
    int clientID, i=0;
    for( ; i<MAX_NUMBER_OF_CLIENTS; i++)
    {
        if (clients[i].queueID == -1) break;
    }
    clientID = i;

    if (clientID == MAX_NUMBER_OF_CLIENTS)
    {
        perror("To mamy clients on server");
        return;
    }

    int clientQueueId;
    sscanf(msgContent, "%i", &clientQueueId);

    clients[clientID].pid = senderId;
    clients[clientID].current_friends_number = 0;
    clients[clientID].queueID = clientQueueId;

    char response[MAX_MESSAGE_LENGTH];
    sprintf(response, "%i", clientID);
    send_response(clientID, INIT, response);
}

void exec_list(int senderId)
{
    printf("Received list request from client %d", senderId);

    char buf[MAX_MESSAGE_LENGTH], response[MAX_MESSAGE_LENGTH];
    int i = 0;
    for ( ; i < MAX_NUMBER_OF_CLIENTS; i++)
    {
        if ( clients[i].queueID != -1)
        {
            sprintf(buf, "Id: %i QueueID: %i\n", i, clients[i].queueID);
            strcat(response,buf);
        }
    }

    send_response(senderId, LIST, response);
}

void exec_friends(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received friends request from client %d", senderId);

    char list_of_friends[MAX_MESSAGE_LENGTH];

    clients[senderId].current_friends_number = 0;

    if (sscanf(msgContent, "%s", list_of_friends) == 1)
    {
        add_friends(senderId, list_of_friends);
    }
}

void exec_2all(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received 2all request from client %d", senderId);

    char response[MAX_MESSAGE_LENGTH];
    char date[31];
    FILE *f = popen("date", "r");
    fread(date, sizeof(char), 31, f);
    pclose(f);

    sprintf(response, "%s ID: %d Date: %s", msgContent, senderId, date);

    int i = 0;
    for (; i<MAX_NUMBER_OF_CLIENTS; i++)
    {
        if (clients[i].queueID != -1) {
            send_response(i, _2ALL, response);
            kill(clients[i].pid, SIGRTMIN);
        }
    }
}

void exec_2friends(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received 2friends request from client %d", senderId);

    char response[MAX_MESSAGE_LENGTH];
    char date[31];
    FILE *f = popen("date", "r");
    fread(date, sizeof(char), 31, f);
    pclose(f);

    sprintf(response, "%s ID: %d Date: %s", msgContent, senderId, date);

    int i = 0;
    for (; i < clients[senderId].current_friends_number; i++)
    {
        send_response(i, _2FRIENDS, response);
        kill(clients[i].pid, SIGRTMIN);
    }
}

void exec_2one(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received 2one request from client %d", senderId);

    char text[MAX_MESSAGE_LENGTH-44], response[MAX_MESSAGE_LENGTH];        //TO DO do something with size of text
    int reciverId;
    char date[31];
    FILE *f = popen("date", "r");
    fread(date, sizeof(char), 31, f);
    pclose(f);

    sscanf(msgContent, "%i %s", &reciverId, text);
    sprintf(response, "%s ID: %i Date: %s\n", text, senderId, date);

    send_response(reciverId, _2ONE, response);
    kill(clients[reciverId].pid, SIGRTMIN);
}

void exec_add(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received add request from client %d", senderId);

    char list_of_friends[MAX_MESSAGE_LENGTH];

    if (sscanf(msgContent, "%s", list_of_friends) == 1)
    {
        add_friends(senderId, list_of_friends);
    }
}

void exec_del(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received del request from client %d", senderId);

    char list_of_friends[MAX_MESSAGE_LENGTH];

    if (sscanf(msgContent, "%s", list_of_friends) == 1)
    {
        del_friends(senderId, list_of_friends);
    }
}

void send_response(int clientID, enum Command_t type, char response[MAX_MESSAGE_LENGTH])
{
    printf("Sending response to client %d", clientID);

    struct Message_t message;
    message.type = type;
    strcpy(message.content, response);
    message.senderId = -1;

    if (msgsnd(clients[clientID].queueID, &message, MSGSZ, IPC_NOWAIT) == -1) perror("Cannot send response to client");
}

void handle_message(struct Message_t *message) {
    switch (message->type) {
        case STOP:
            exec_stop(message->senderId);
            break;
        case ECHO:
            exec_echo(message->senderId, message->content);
            break;
        case INIT:
            exec_init(message->senderId, message->content);
            break;
        case LIST:
            exec_list(message->senderId);
            break;
        case FRIENDS:
            exec_friends(message->senderId, message->content);
            break;
        case _2ALL:
            exec_2all(message->senderId, message->content);
            break;
        case _2FRIENDS:
            exec_2friends(message->senderId, message->content);
            break;
        case _2ONE:
            exec_2one(message->senderId, message->content);
            break;
        case ADD:
            exec_add(message->senderId, message->content);
            break;
        case DEL:
            exec_del(message->senderId, message->content);
            break;
    }
}

void finishWork()
{
    printf("Turning off the server");

    int i = 0;
    for (; i < MAX_NUMBER_OF_CLIENTS; i++) {
        if (clients[i].queueID != -1) {
            kill(clients[i].pid, SIGINT);
        }
    }

    if (msgctl(queueID, IPC_RMID, NULL) == -1){
        perror("Cannot remove server queue");
        return;
    }

    stop = 1;
}

int main()
{
    signal(SIGINT, finishWork);
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
    while(!stop)
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