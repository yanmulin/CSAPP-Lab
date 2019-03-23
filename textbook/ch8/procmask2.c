#include "csapp.h"
#define MAXJOBS 128

pid_t jobs[MAXJOBS];
unsigned long jobsHead = 0, jobsTail = 0;

void addjob(pid_t pid); 
void deletejob(pid_t pid); 

void handler1(int signum) {
    sigset_t mask, prev_mask;
    int olderrno = errno;
    sigfillset(&mask);
    pid_t pid;
    while ((pid=waitpid(-1, NULL, 0)) > 0) {
        sigprocmask(SIG_BLOCK, &mask, &prev_mask);
        deletejob(pid);
        sigprocmask(SIG_SETMASK, &prev_mask, NULL);
    }
    errno = olderrno;
}

int main(int argc, char **argv) {
    pid_t pid;
    sigset_t mask_one, mask_all, prev_one, prev_all;
    sigfillset(&mask_all);
    sigemptyset(&mask_one);
    sigaddset(&mask_one, SIGCHLD);

    if (signal(SIGCHLD, handler1) == SIG_ERR) 
        unix_error("signal");
    
    while (1) {
        sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
        if ((pid=Fork()) == 0) {
            sigprocmask(SIG_SETMASK, &prev_one, NULL);
            Execve("/bin/date", argv, environ);
        }
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        addjob(pid);
        sigprocmask(SIG_SETMASK, &prev_one, NULL);
    } 

    exit(0);
    return 0;
}

void addjob(pid_t pid) {
    if (pid > 0 &&  (jobsTail + 1) != jobsHead){
        jobs[jobsTail] = pid;
        jobsTail = (jobsTail + 1) % MAXJOBS;
    }
}

void deletejob(pid_t pid) {
    int i;
    for (i=jobsHead;i!=jobsTail;i++){
        if (jobs[i] == pid) break;
    }
    if (i != jobsTail) {
        for (;i!=jobsTail;i++)
            jobs[i] = jobs[(i+1)%MAXJOBS];
    }
}