#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h> 
#include <libgen.h>
#include <time.h>
#include <signal.h>

int received_signals = 0;
int sent_signals;

void count_function(int sig,siginfo_t *info, void *void_ptr)
{
    received_signals++;
    printf("Odebrano sygnal nr %d od catchera \n",info->si_value.sival_int);
}

void signal_USR1(int sig_num)
{
    received_signals++;
}

void signal_USR2(int sig_num)
{
    printf("Number of received signals %d\n", received_signals);
    printf("Number of sent signals %d\n", sent_signals);
    exit(0);
}

void signal_1(int sig_num)
{
    received_signals++;
    return;
}

void signal_2(int sig_num)
{
    printf("Number of received signals %d\n", received_signals);
    printf("Number of sent signals %d\n", sent_signals);
    exit(0);
}


void send_with_kill(int num_of_signals, int catcher_pid)
{
    while (num_of_signals > 0)
    {
        kill(catcher_pid,SIGUSR1 );
        num_of_signals--;
    }
    kill(catcher_pid,SIGUSR2 );
}

void send_with_queue(int num_of_signals, int catcher_pid, union sigval value)
{
    
    while (num_of_signals > 0)
    {
        sigqueue(catcher_pid, SIGUSR1, value);
        num_of_signals--;
    }
    sigqueue(catcher_pid, SIGUSR2, value);
}

void send_with_sigrt(int num_of_signals, int catcher_pid)
{
    while (num_of_signals > 0)
    {
        kill(catcher_pid, SIGRTMIN+1);
        num_of_signals--;
    }
    kill(catcher_pid, SIGRTMIN+2);
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("Expected 3 arguments\n");
        return -1;
    }

    int catcher_pid;
    if(sscanf(argv[1],"%d", &catcher_pid) != 1)
    {
        printf("First parametr should be integer\n");
        return -1;
    }

    int num_of_signals;
    if(sscanf(argv[2],"%d", &num_of_signals) != 1)
    {
        printf("Second parametr should be integer\n");
        return -1;
    }
    sent_signals = num_of_signals;

    char *mode = malloc(20 * sizeof(char));
    mode = argv[3];

    struct sigaction act;
    union sigval value;

    if ( strcmp(mode,"KILL") == 0)
    {
        act.sa_handler = signal_USR1;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaddset(&act.sa_mask, SIGUSR2);
        sigaction(SIGUSR1, &act, NULL);
        act.sa_handler = signal_USR2;
        sigaction(SIGUSR2, &act, NULL);

        send_with_kill(num_of_signals, catcher_pid);
    }
    else if ( strcmp(mode,"SIGQUEUE") == 0)
    {
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = &count_function;
        sigemptyset(&act.sa_mask);
        sigaddset(&act.sa_mask, SIGUSR2);
        sigaction(SIGUSR1, &act, NULL);
        act.sa_flags = 0;
        act.sa_handler = signal_USR2;
        sigaction(SIGUSR2, &act, NULL);

        send_with_queue(num_of_signals, catcher_pid, value);
    }
    else if ( strcmp(mode,"SIGRT") == 0)
    {
        act.sa_handler = signal_1;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaddset(&act.sa_mask, SIGRTMIN+1);
        sigaction(SIGRTMIN+1, &act, NULL);
        act.sa_handler = signal_2;
        sigaction(SIGRTMIN+2, &act, NULL);

        send_with_sigrt(num_of_signals, catcher_pid);
    }
    else
    {
        printf("Incorrect mode argument\n");
        return -1;
    }

    while(1){
        sleep(1);
    }

    return 0;
}