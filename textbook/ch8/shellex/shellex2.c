#include "csapp.h"
#include <assert.h>

#define MAXJOBS 12
#define MAXARGS 12

// declarations
typedef enum {Deleted, Stopped, Running, Done} JobState;
typedef struct {
    pid_t pid;
    char cmd[MAXLINE];
    JobState state;
}Job;
void printJobs();
void cleanJobs();
void waitDone(int jobIdx);
int addJob(pid_t pid, const char *cmd);
int findJob(pid_t pid);
void fgJob(const char *arg);
void bgJob(const char *arg);
int builtinCommand(char *argv[]);
int commandParser(char *cmdline, char *argv[]);
void eval(char *line);

// jobs data structure
Job jobs[MAXJOBS];
int jobsNum = 0, fgPtr = -1;

// signal masks
sigset_t mask_all, prev_all, mask_one, prev_one;
sigset_t s_mask_all, s_prev_all;

// signal handler
void sigchld_handler(int signo) {
    int olderrno = errno;
    int status;
    pid_t pid;
    while ((pid=waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0) {
        sigprocmask(SIG_BLOCK, &s_mask_all, &s_prev_all);
        int jobIdx = findJob(pid);
        assert (jobsNum > jobIdx && jobIdx >= 0);
        // sio_putl(pid);
        // sio_puts(" child\n");
        if (WIFEXITED(status) || WIFSIGNALED(status))
            jobs[jobIdx].state = Done;
        else if (WIFSTOPPED(status)) 
            jobs[jobIdx].state = Stopped;
        sigprocmask(SIG_SETMASK, &s_prev_all, NULL);
    }
    errno = olderrno;
}
void sigint_handler(int signo) {
    int olderrno = errno;
    sigprocmask(SIG_BLOCK, &s_mask_all, &s_prev_all);
    if (jobs[fgPtr].state == Running)
        kill(jobs[fgPtr].pid, signo);
    sigprocmask(SIG_SETMASK, &s_prev_all, NULL);
    errno = olderrno;
}
void sigtstp_handler(int signo) {
    int olderrno = errno;
    sigprocmask(SIG_BLOCK, &s_mask_all, &s_prev_all);
    if (jobs[fgPtr].state == Running)
        kill(jobs[fgPtr].pid, signo);
    sigprocmask(SIG_SETMASK, &s_prev_all, NULL);
    errno = olderrno;
}

int main() {
    char line[MAXLINE];

    sigfillset(&mask_all);
    sigfillset(&s_mask_all);
    sigemptyset(&mask_one);
    sigaddset(&mask_one, SIGCHLD);

    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, sigtstp_handler);    

// printf("pid #%d, pgid #%d\n", getpid(), getpgid(0));

    while (1) {
        printf("> ");
        Fgets(line, MAXLINE, stdin);
        if (feof(stdin)) exit(0);
        eval(line);
    }
    return 0;
}

// command line handler
void eval(char *line) {
    char buf[MAXLINE];
    char *argv[MAXARGS];

    line[strlen(line)-1] = ' ';
    strcpy(buf, line);
    
    int bg = commandParser(buf, argv);

    if (builtinCommand(argv) == 0) { 
        sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
        pid_t pid = Fork();
        if (pid == 0) {
            sigprocmask(SIG_SETMASK, &prev_one, NULL);
            setpgid(0,0);
            // printf("In child #%d, pgid:%d\n", getpid(), getpgid(0));
            // printf("foreground pgid: %d\n", tcgetpgrp(STDIN_FILENO));
            // tcsetpgrp(STDOUT_FILENO, getppid());
            Execve(argv[0], argv, environ);
        }
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        int jobIdx = addJob(pid, line);
        if (!bg) fgPtr = jobIdx;
        // sigprocmask(SIG_SETMASK, &prev_all, NULL);
        sigprocmask(SIG_SETMASK, &prev_one, NULL);

        if (!bg) waitDone(jobIdx);
        else printf("[%d] %u\n", jobIdx + 1, pid);
    }
    cleanJobs();
}

int commandParser(char *cmdline, char *argv[]) {
    int argc = 0;
    int bg = 0;
    char *ptr = cmdline;

    // skip space
    while (*ptr && *ptr == ' ') ptr ++;

    argv[argc++] = ptr;
    while ((ptr=strchr(ptr, ' ')) != NULL) {
        *ptr++ = '\0';
        while (*ptr && *ptr == ' ') ptr++;
        if (*ptr) argv[argc++] = ptr;
    } 
    
    if (strcmp(argv[argc-1], "&") == 0){
        argv[argc-1] = NULL;
        bg = 1;
    }else {
        argv[argc] = NULL;
    }
    return bg;
}

int builtinCommand(char *argv[]) {
    if (strcmp(argv[0], "jobs") == 0) 
        printJobs();
    else if (strcmp(argv[0], "fg") == 0) 
        fgJob(argv[1]);
    else if (strcmp(argv[0], "bg") == 0) 
        bgJob(argv[1]);
    else if (strcmp(argv[0], "quit") == 0)
        exit(0);
    else if (strcmp(argv[0], "") == 0) ;
    else return 0;
    return 1;
}

// jobs handler
int addJob(pid_t pid, const char *cmd) {
    int i;
    for (i=0;i<jobsNum&&jobs[i].state!=Deleted;i++);
    if (i == MAXJOBS) unix_error("max jobs");
    jobs[i].pid = pid;
    strcpy(jobs[i].cmd, cmd);
    jobs[i].state = Running;
    if (i == jobsNum) jobsNum ++;
    return i;
}

void waitDone(int jobIdx) {
    sigset_t mask, prev;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &prev);
    // printf("Wait\n");
    while (jobs[jobIdx].state == Running) 
        sigsuspend(&prev_one);
    sigprocmask(SIG_SETMASK, &prev, NULL);
}

void cleanJobs() {
    int i;
    for (i=jobsNum-1;i>=0&&jobs[i].state==Done;i--)
        jobsNum --;
    for (;i>=0;i--) {
        if (jobs[i].state == Done) 
            jobs[i].state = Deleted;
    }
}

void printJobs() {
    for (int i=0;i<jobsNum;i++) {
        if (jobs[i].state == Deleted) continue;
        printf("[%d] ", i+1);
        printf("%u ", jobs[i].pid);
        switch(jobs[i].state) {
            case Running:printf("Running\t");break;
            case Stopped:printf("Stopped\t");break;
            case Done:printf("Done\t");break;
            default:assert(0);
        }
        printf("%s\n", jobs[i].cmd);
    }
}

int findJob(pid_t pid) {
    for (int i=0;i<jobsNum;i++){
        if (jobs[i].state != Deleted && jobs[i].pid == pid)
            return i;
    }
    return -1;
}
void fgJob(const char *arg) {
    sigset_t mask, prev;
    sigfillset(&mask);
    if (arg == NULL) fprintf(stderr, "no such job\n");
    else {
        int id = -1;
        sigprocmask(SIG_BLOCK, &mask, &prev);
        if (arg[0] != '%') {
            int pid = atoi(&arg[0]);
            if (pid > 0) id = findJob(pid);
        } else id = atoi(&arg[1]) - 1;

        if (id >= 0 && id < jobsNum && jobs[id].state != Deleted) {
            fgPtr = id;
            kill(jobs[id].pid, SIGCONT);
            jobs[id].state = Running;
            waitDone(id);
        } else fprintf(stderr, "no such job\n");
        sigprocmask(SIG_SETMASK, &prev, NULL);
    }
}

void bgJob(const char *arg) {
    sigset_t mask, prev;
    sigfillset(&mask);
    if (arg == NULL) fprintf(stderr, "no such job\n");
    else {
        sigprocmask(SIG_BLOCK, &mask, &prev);
        int id = -1;
        if (arg[0] != '%') {
            int pid = atoi(&arg[0]);
            if (pid > 0) id = findJob(pid);
        } else id = atoi(&arg[1]) - 1;

        if (id >= 0 && id < jobsNum && jobs[id].state != Deleted) {
            jobs[id].state = Running;
            kill(jobs[id].pid, SIGCONT);
        } else fprintf(stderr, "no such job\n");
        sigprocmask(SIG_SETMASK, &prev, NULL);
    }
}