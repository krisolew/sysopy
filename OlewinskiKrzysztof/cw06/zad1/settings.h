#include <stdio.h>
#include <unistd.h>

#define MAX_NUMBER_OF_CLIENTS 20
#define MEX_MESSAGE_LENGTH 500
#define MAX_COMMAND_LENGTH 500
#define NUMBER_OF_COMMANDS 10

enum Command_t
{
    STOP = 1,
    INIT = 2,
    ECHO = 3,
    LIST = 4,
    FIRENDS = 5,
    ADD = 6,
    DEL = 7,
    _2ALL = 8,
    _2FRIENDS = 9,
    _2ONE = 10
};

struct Message_t
{
    long msgType;
    pid_t senderId;
    char msgContent[MEX_MESSAGE_LENGTH];
};