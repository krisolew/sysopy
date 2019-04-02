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

int main()
{
    sigset_t set, oldset;
    sigemptyset(&oldset);
    sigfillset(&set);
    sigdelset(&set, SIGUSR1);
    sigdelset(&set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &set, &oldset);

    printf("PID catchera: %d\n", getpid());
    
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaddset(&act.sa_mask, SIGUSR2);
    act.sa_sigaction = &count_function;
    sigaction(SIGUSR1, &act, NULL);
    act.sa_flags = 0;
    act.sa_handler = signal_USR2;
    sigaction(SIGUSR2, &act, NULL);

    while(1) {
        sleep(1);
    }

    return 0;
}