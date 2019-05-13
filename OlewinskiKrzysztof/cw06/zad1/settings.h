#include <stdio.h>
#include <unistd.h>

#define MAX_NUMBER_OF_CLIENTS 20
#define MAX_MESSAGE_LENGTH 500
#define MAX_COMMAND_LENGTH 500
#define NUMBER_OF_COMMANDS 10
#define SERVER_KEY 's'

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

struct Message_t
{
    long type;
    pid_t senderId;
    char content[MAX_MESSAGE_LENGTH];
};

#define MSGSZ sizeof(struct Message_t)-sizeof(long) //msgsz doesn't contain mType

key_t getServerQueueKey() {
    char *homeDir = getenv("HOME");
    if (homeDir == NULL) perror("No HOME environment variable");
    return ftok(homeDir, SERVER_KEY);
}

key_t getClientQueueKey() {
    char *homeDir = getenv("HOME");
    if (homeDir == NULL) perror("No HOME environment variable");
    return ftok(homeDir, getpid());
}