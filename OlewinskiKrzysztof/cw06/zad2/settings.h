#include <stdio.h>
#include <unistd.h>

#define MAX_NUMBER_OF_CLIENTS 20
#define MAX_MESSAGE_LENGTH 500
#define NUMBER_OF_COMMANDS 10
#define MAX_QUEUE_SIZE 10
#define SERVER_NAME "/serverQueue"

enum Command_t
{
    STOP = 1,
    INIT = 2,
    ECHO = 3,
    LIST = 4,
    FRIENDS = 5,
    ADD = 6,
    DEL = 7,
    _2ALL = 8,
    _2FRIENDS = 9,
    _2ONE = 10
};

int getCommandPriority(enum Command_t command)
{
    switch(command)
    {
        case STOP:
            return 4;
        case INIT:
            return 3;
        case FRIENDS:
        case ADD:
        case DEL:
        case LIST:
            return 2;
        default:
            return 1;
    }
}

struct Message_t
{
    long type;
    pid_t senderId;
    char content[MAX_MESSAGE_LENGTH];
};

#define MSGSZ sizeof(struct Message_t)-sizeof(long) //msgsz doesn't contain mType

char* getClientQueueName() {
    char *name = malloc(20 * sizeof(char));
    sprintf(name, "client%i%i", getpid(), rand()%1024);
    return name;
}