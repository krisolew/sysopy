#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>

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

void send_response(int clientID, enum Command_t type, char response[MAX_MESSAGE_LENGTH]);

void add_friends(int clientID, char list_of_friends[MAX_MESSAGE_LENGTH])
{
    char *friend = strtok(list_of_friends, "|");

    while (friend != NULL && clients[clientID].current_friends_number < MAX_NUMBER_OF_CLIENTS)
    {
        int friendID = strtol(friend, NULL, 10);
        int exists = 0, i = 0;

        if (clients[friendID].queueID == -1 || friendID < 0 || friendID > MAX_NUMBER_OF_CLIENTS)
        {
            printf("Friend doest not exists\n");
            friend = strtok(NULL, "|");
            continue;
        }

        if (clientID == friendID)
        {
            printf("You cannot add yourself to your friends\n");
            friend = strtok(NULL, "|");
            continue;
        }

        for (; i < clients[clientID].current_friends_number; i++)
        {
            if (friendID == clients[clientID].friends[i]) exists = 1;
        }

        if (exists)
        {
            printf("Friend %d has already been your firend\n", friendID);
            friend = strtok(NULL, "|");
            continue;
        }

        clients[clientID].friends[clients[clientID].current_friends_number] = friendID;
        clients[clientID].current_friends_number++;

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

        if (clients[friendID].queueID == -1 || friendID < 0 || friendID > MAX_NUMBER_OF_CLIENTS)
        {
            printf("Friend doest not exists\n");
            friend = strtok(NULL, "|");
            continue;
        }

        if (clientID == friendID)
        {
            printf("You cannot add yourself to your friends\n");
            friend = strtok(NULL, "|");
            continue;
        }

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
    printf("Received stop request from client %d\n", senderId);

    if ( senderId >=0 && senderId < MAX_NUMBER_OF_CLIENTS)
    {
      if (mq_close(clients[senderId].queueID) == -1)
      {
         perror("Cannot close server queue");
         return;
      }

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
    printf("Received echo request from client %d\n", senderId);

    char response[MAX_MESSAGE_LENGTH] = "";
    char date[31] = "";
    FILE *f = popen("date", "r");
    fread(date, sizeof(char), 31, f);
    pclose(f);
    sprintf(response, "%s Date: %s", msgContent, date);
    send_response(senderId, ECHO, response);
}

void exec_init(pid_t senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received init request from client %d\n", senderId);

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

    if ((clients[clientID].queueID = mq_open(msgContent, O_WRONLY)) == -1)
    {
        perror("Cannot open client queue");
        return;
    }

    clients[clientID].pid = senderId;
    clients[clientID].current_friends_number = 0;

    char response[MAX_MESSAGE_LENGTH] = "";
    sprintf(response, "%i", clientID);
    send_response(clientID, INIT, response);
}

void exec_list(int senderId)
{
    printf("Received list request from client %d\n", senderId);

    char buf[MAX_MESSAGE_LENGTH] = "";
    char response[MAX_MESSAGE_LENGTH] = "";
    int i = 0;
    for ( ; i < MAX_NUMBER_OF_CLIENTS; i++)
    {
        if ( clients[i].queueID != -1)
        {
            sprintf(buf, "Id: %i queueID: %i\n", i, clients[i].queueID);
            strcat(response,buf);
        }
    }

    send_response(senderId, LIST, response);
}

void exec_friends(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received friends request from client %d\n", senderId);

    char list_of_friends[MAX_MESSAGE_LENGTH] = "";

    clients[senderId].current_friends_number = 0;

    if (sscanf(msgContent, "%s", list_of_friends) == 1)
    {
        add_friends(senderId, list_of_friends);
    }
}

void exec_2all(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received 2all request from client %d\n", senderId);

    char response[MAX_MESSAGE_LENGTH] = "";
    char date[31] = "";
    FILE *f = popen("date", "r");
    fread(date, sizeof(char), 31, f);
    pclose(f);

    sprintf(response, "%s ID: %d Date: %s", msgContent, senderId, date);

    int i = 0;
    for (; i<MAX_NUMBER_OF_CLIENTS; i++)
    {
        if (clients[i].queueID != -1 && i != senderId) {
            send_response(i, _2ALL, response);
            kill(clients[i].pid, SIGRTMIN);
        }
    }
}

void exec_2friends(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received 2friends request from client %d\n", senderId);

    char response[MAX_MESSAGE_LENGTH] = "";
    char date[31] = "";
    FILE *f = popen("date", "r");
    fread(date, sizeof(char), 31, f);
    pclose(f);

    sprintf(response, "%s ID: %d Date: %s", msgContent, senderId, date);

    int i = 0;
    for (; i < clients[senderId].current_friends_number; i++)
    {
        send_response(clients[senderId].friends[i], _2FRIENDS, response);
        kill(clients[clients[senderId].friends[i]].pid, SIGRTMIN);
    }
}

void exec_2one(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received 2one request from client %d\n", senderId);

    char text[MAX_MESSAGE_LENGTH-44] = "", response[MAX_MESSAGE_LENGTH] = "";        //TO DO do something with size of text
    int reciverId;
    char date[31] = "";
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
    printf("Received add request from client %d\n", senderId);

    char list_of_friends[MAX_MESSAGE_LENGTH] = "";

    if (sscanf(msgContent, "%s", list_of_friends) == 1)
    {
        add_friends(senderId, list_of_friends);
    }
}

void exec_del(int senderId, char msgContent[MAX_MESSAGE_LENGTH])
{
    printf("Received del request from client %d\n", senderId);

    char list_of_friends[MAX_MESSAGE_LENGTH] = "";

    if (sscanf(msgContent, "%s", list_of_friends) == 1)
    {
        del_friends(senderId, list_of_friends);
    }
}

void send_response(int clientID, enum Command_t type, char response[MAX_MESSAGE_LENGTH])
{
    printf("Sending response to client %d\n", clientID);

    struct Message_t message;
    message.type = type;
    strcpy(message.content, response);
    message.senderId = -1;

    if (mq_send(clients[clientID].queueID, (char *) &message, MAX_MESSAGE_LENGTH, getCommandPriority(type)) == -1) perror("Cannot send response to client");
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
    printf("\nTurning off the server\n");

    int i = 0;
    for (; i < MAX_NUMBER_OF_CLIENTS; i++) {
        if (clients[i].queueID != -1) {
            kill(clients[i].pid, SIGUSR1);
        }
    }

    if (mq_close(queueID) == -1)
    {
        perror("Cannot close server queue");
        exit(-1);
    }
    if (mq_unlink(SERVER_NAME) == -1){
        perror("Cannot remove server queue");
        exit(-1);
    }

    exit(0);
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

    struct mq_attr queue_attr;
    queue_attr.mq_maxmsg = MAX_QUEUE_SIZE;
    queue_attr.mq_msgsize = MAX_MESSAGE_LENGTH;

    if ((queueID = mq_open(SERVER_NAME, O_RDONLY | O_CREAT | O_EXCL, 0666, &queue_attr)) == -1)
    {
        perror("Cannot create server queue");
        return -1;
    }

    struct Message_t message;
    while(1)
    {
        if (mq_receive(queueID, (char *) &message, MAX_MESSAGE_LENGTH, NULL) == -1)
        {
            perror("Cannot receive message form queue");
            finishWork();
        }
        handle_message(&message);
    }

    return 0;
}
