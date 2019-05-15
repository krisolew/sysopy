#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>

#include "settings.h"

int serverQueueID = -1;
int clientQueueID = -1;
int clientID = -1;
char *queueName;

int execute_command(FILE *file);
void finish_work();

void send_request(enum Command_t type, char request[MAX_MESSAGE_LENGTH])
{
    struct Message_t message;
    message.type = type;
    strcpy(message.content, request);
    message.senderId = clientID;

    if (mq_send(serverQueueID, (char *) &message, MAX_MESSAGE_LENGTH, getCommandPriority(type)) == -1)
    {
        perror("Cannot send request to server");
        return;
    }
}

void receive_response(struct Message_t *message)
{
    if (mq_receive(clientQueueID, (char *) message, MAX_MESSAGE_LENGTH, NULL) == -1)
    {
        perror("Cannot receive server response");
    }
}

void exec_init()
{
    struct Message_t message;

    message.type = INIT;
    strcpy(message.content, queueName);
    message.senderId = getpid();

    if (mq_send(serverQueueID, (char *) &message, MAX_MESSAGE_LENGTH, getCommandPriority(INIT)) == -1)
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
    send_request(STOP, "");
    finish_work();
}

void exec_echo(char args[MAX_MESSAGE_LENGTH])
{
    char command[MAX_MESSAGE_LENGTH] = "", text[MAX_MESSAGE_LENGTH] = "";
    int numberOfArguments = sscanf(args, "%s %s", command, text);
    if (numberOfArguments == EOF || numberOfArguments < 2) {
        printf("Echo expects one argument");
        return;
    }

    send_request(ECHO, text);
    struct Message_t message;
    receive_response(&message);

    if (message.type != ECHO)
    {
        perror("Wrong type of response");
        return;
    }

    printf("%s\n", message.content);
}

void exec_list()
{
    send_request(LIST, "");
    struct Message_t message;
    receive_response(&message);
    if (message.type != LIST)
    {
        perror("Wrong type of response");
        return;
    }

    printf("%s\n", message.content);
}

void exec_friends(char args[MAX_MESSAGE_LENGTH])
{
    char command[MAX_MESSAGE_LENGTH] = "", text[MAX_MESSAGE_LENGTH] = "";
    int numberOfArguments = sscanf(args, "%s %s", command, text);
    if (numberOfArguments == EOF || numberOfArguments == 0)
    {
        perror("Cannot read arguments");
        return;
    }
    send_request(FRIENDS, text);
}

void exec_2one(char args[MAX_MESSAGE_LENGTH])
{
    char command[MAX_MESSAGE_LENGTH] = "", text[MAX_MESSAGE_LENGTH] = "", receiverId[MAX_MESSAGE_LENGTH] = "";
    int numberOfArguments = sscanf(args, "%s %s %s", command, receiverId, text);
    if (numberOfArguments == EOF || numberOfArguments < 3)
    {
        perror("2one expects two arguments");
        return;
    }
    send_request(_2ONE, strcat(receiverId, text));
}

void exec_2friends(char args[MAX_MESSAGE_LENGTH])
{
    char command[MAX_MESSAGE_LENGTH] = "", text[MAX_MESSAGE_LENGTH] = "";
    int numberOfArguments = sscanf(args, "%s %s", command, text);
    if (numberOfArguments == EOF || numberOfArguments < 2)
    {
        perror("2friends expects one arguments");
        return;
    }
    send_request(_2FRIENDS, text);
}

void exec_2all(char args[MAX_MESSAGE_LENGTH])
{
    char command[MAX_MESSAGE_LENGTH] = "", text[MAX_MESSAGE_LENGTH] = "";
    int numberOfArguments = sscanf(args, "%s %s", command, text);
    if (numberOfArguments == EOF || numberOfArguments < 2)
    {
        perror("2all expects one arguments");
        return;
    }
    send_request(_2ALL, text);
}

void exec_add(char args[MAX_MESSAGE_LENGTH])
{
    char command[MAX_MESSAGE_LENGTH] = "", text[MAX_MESSAGE_LENGTH] = "";
    int numberOfArguments = sscanf(args, "%s %s", command, text);
    if (numberOfArguments == EOF || numberOfArguments < 2)
    {
        perror("2all expects one arguments");
        return;
    }
    send_request(ADD, text);
}

void exec_del(char args[MAX_MESSAGE_LENGTH])
{
    char command[MAX_MESSAGE_LENGTH] = "", text[MAX_MESSAGE_LENGTH] = "";
    int numberOfArguments = sscanf(args, "%s %s", command, text);
    if (numberOfArguments == EOF || numberOfArguments < 2)
    {
        perror("2all expects one arguments");
        return;
    }
    send_request(DEL, text);
}

void exec_read(char args[MAX_MESSAGE_LENGTH])
{
    char command[MAX_MESSAGE_LENGTH] = "", fileName[MAX_MESSAGE_LENGTH] = "";
    int numberOfArguments = sscanf(args, "%s %s", command, fileName);
    if (numberOfArguments == EOF || numberOfArguments < 2) {
        printf("Read expects file name");
        return;
    }

    FILE *f = fopen(fileName, "r");
    if (f == NULL)
    {
        perror("Cannot open file");
        return;
    }

    while (execute_command(f) != EOF){}
    fclose(f);
}

int execute_command(FILE *file)
{
    char args[MAX_MESSAGE_LENGTH] = "", command[MAX_MESSAGE_LENGTH] = "";
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

void finish_work()
{
    if (mq_close(serverQueueID) == -1)
    {
        perror("Cannot close server queue");
        exit(-1);
    }
    if (mq_close(clientQueueID) == -1)
    {
        perror("Cannot close client queue");
        exit(-1);
    }
    if (mq_unlink(queueName) == -1)
    {
        perror("Cannot remove client queue");
        exit(-1);
    }

    exit(0);
}

void exit_signal()
{
    exec_stop();
    finish_work();
}

void signal_handler(int signalno)
{
    struct Message_t message;
    receive_response(&message);
    switch (message.type) {
        case _2ALL:
        case _2FRIENDS:
        case _2ONE:
            printf("%s", message.content);
            break;
        case STOP:
            finish_work();
            break;
    }
}

int main()
{
    signal(SIGINT, exit_signal);
    signal(SIGUSR1, finish_work);
    signal(SIGRTMIN, signal_handler);

    queueName = getClientQueueName();

    if ((serverQueueID = mq_open(SERVER_NAME, O_WRONLY)) == -1)
    {
        perror("Cannot create server queue");
        return -1;
    }

    struct mq_attr queue_attr;
    queue_attr.mq_maxmsg = MAX_QUEUE_SIZE;
    queue_attr.mq_msgsize = MAX_MESSAGE_LENGTH;

    if ((clientQueueID = mq_open(queueName, O_RDONLY | O_CREAT | O_EXCL, 0666, &queue_attr)) == -1)
    {
        perror("Cannot create client queue");
        return -1;
    }

    exec_init();

    while(1)
    {
        execute_command(fdopen(STDIN_FILENO, "r"));
    }
    
    return 0;
}