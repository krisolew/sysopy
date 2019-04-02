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
int sender_pid = -1;

void count_function(int sig,siginfo_t *info, void *void_ptr){
    received_signals++;
    sender_pid = info->si_pid;
}

void signal_USR2(int sig_num)
{
    if (sender_pid == -1) exit(0);
    int i = received_signals;
    for (; i>0; i--)
    {
        kill(sender_pid, SIGUSR1);
    }

    kill(sender_pid, SIGUSR2);
    printf("Number of received signals from sender %d\n", received_signals);
    exit(0);
}

void send_with_queue(int sig_num)
{
    union sigval value;
    value.sival_int = 1;

    if (sender_pid == -1) exit(0);
    int i = received_signals;
    while (i> 0)
    {
        sigqueue(sender_pid, SIGUSR1, value);
        i--;
        value.sival_int++;
    }
    sigqueue(sender_pid, SIGUSR2, value);
    printf("Number of received signals from sender %d\n", received_signals);
    exit(0);
}

void signal_2(int sig_num)
{
    if (sender_pid == -1) exit(0);
    int i = received_signals;
    for (; i>0; i--)
    {
        kill(sender_pid, SIGRTMIN+1);
    }

    kill(sender_pid, SIGRTMIN+2);
    printf("Number of received signals from sender %d\n", received_signals);
    exit(0);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Expected 1 argument\n");
        return -1;
    }

    char *mode = malloc(20 * sizeof(char));
    mode = argv[1];

    sigset_t set, oldset;
    sigemptyset(&oldset);
    sigfillset(&set);
    sigdelset(&set, SIGUSR1);
    sigdelset(&set, SIGUSR2);
    sigdelset(&set, SIGRTMIN+2);
    sigdelset(&set, SIGRTMIN+1);
    sigprocmask(SIG_BLOCK, &set, &oldset);

    printf("PID catchera: %d\n", getpid());
    

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    
   
    if ( strcmp(mode,"KILL") == 0)
    {
        act.sa_flags = SA_SIGINFO;
        sigaddset(&act.sa_mask, SIGUSR2);
        act.sa_sigaction = &count_function;
        sigaction(SIGUSR1, &act, NULL);
        act.sa_flags = 0;
        act.sa_handler = signal_USR2;
        sigaction(SIGUSR2, &act, NULL);
    }
    else if ( strcmp(mode,"SIGQUEUE") == 0)
    {
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = &count_function;
        sigemptyset(&act.sa_mask);
        sigaddset(&act.sa_mask, SIGUSR2);
        sigaction(SIGUSR1, &act, NULL);
        act.sa_flags = 0;
        act.sa_handler = send_with_queue;
        sigaction(SIGUSR2, &act, NULL);

    }
    else if ( strcmp(mode,"SIGRT") == 0)
    {
        act.sa_flags = SA_SIGINFO;
        sigaddset(&act.sa_mask, SIGRTMIN+2);
        act.sa_sigaction = &count_function;
        sigaction(SIGRTMIN+1, &act, NULL);
        act.sa_flags = 0;
        act.sa_handler = signal_2;
        sigaction(SIGRTMIN+2, &act, NULL);
    }
    else
    {
        printf("Incorrect mode argument\n");
        return -1;
    }


    while(1) {
        sleep(1);
    }

    return 0;
}