#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int stp = 0;
pid_t pid;

void child(){
    sigset_t new_set;
    sigset_t old_set;
    sigemptyset(&new_set);
    sigfillset(&new_set);
    if ((sigprocmask(SIG_BLOCK,&new_set, &old_set)) < 0)
        perror("Nie udało się ");

    execl("./loop.sh", "loop.sh");      
    return;
}

void signal_stp(int signal_num)
{
    if ( stp == 0) 
    {
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n");
        stp = 1;
        kill(pid, SIGKILL);
    
    }
    else
    {
        stp = 0;
        pid = fork();
        if(pid==0) child();
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
    pid = fork();

    if(pid==0){
        sleep(1);
        child();
     }
    else{
        struct sigaction act;
        struct sigaction old_act;

        act.sa_handler = signal_int;

        signal(SIGTSTP, signal_stp);
        sigaction(SIGINT, &act, &old_act);
        while(1){
            sleep(1);
        }
    }

    return 0;
}