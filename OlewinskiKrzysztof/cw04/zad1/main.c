#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int stp = 0;

void signal_stp(int signal_num)
{
    if ( stp == 0) 
    {
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
        stp = 1;
    }
    else
    {
        stp = 0;
        raise(SIGALRM);
    }
}

void signal_alrm(int signal_num)
{
    return;
}

void signal_int(int signal_num)
{
    printf("\nOdebrano sygnał SIGINT\n");
    exit(0);
}

int main()
{
    struct sigaction act;
    struct sigaction old_act;

    act.sa_handler = signal_int;

    signal(SIGTSTP, signal_stp);
    signal(SIGALRM, signal_alrm);
    sigaction(SIGINT, &act, &old_act);

    while (1)
    {
        system("date");
        sleep(1);
        if (stp == 1) pause();
    }

    return 0;
}