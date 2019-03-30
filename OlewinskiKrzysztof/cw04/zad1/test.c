#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>


int i= 0;

void sighandler(int signum){
    i++;
    printf("numer: %d\t", i);
    printf("sygnal: %d\n", signum);
}

void au(int sig_no) {
    printf("Otrzymale signal %d.\n", sig_no);
}

int main() {

    pid_t pid = fork();
    int i = 0;
    if(pid!=0){
        while(1>0){
            sleep(1);
            kill(pid, SIGUSR1);
            sleep(1);
            kill(pid, SIGUSR2);
            sleep(1);
            kill(pid, SIG_BLOCK);
            sleep(1);
            kill(pid, SIGABRT);
            sleep(1);
            kill(pid, SIG_IGN);
            sleep(1);
            //kill(pid, SIGKILL);
        }
    }
    else{
        struct sigaction act;
        act.sa_handler = au;
        sigset_t old_set;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

       sigfillset(&act.sa_mask);
        // // const sigval s_val;
        // // s_val.sival_int = rand();
        // // sigqueue(pid, SIGUSR1,s_val.sival_int);

        // //sigaction()

        if ((sigprocmask(SIG_BLOCK,&act.sa_mask, &old_set)) < 0)
            perror("Nie udało się ");

        while(1>0){
            sleep(1);
            printf("%d\n", getpid());
        }
    }

    return 0;
}