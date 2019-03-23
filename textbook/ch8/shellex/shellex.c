#include "csapp.h"
#include <assert.h>

#define LINEMAX 128
#define MAXARGS 32
#define MAXJOBS 32

typedef enum {
    Deleted, Running, Stopped, 
}JobStatus;
typedef struct {
    char cmdline[LINEMAX];
    JobStatus status;
    pid_t pid;
}Job;

Job jobs[MAXJOBS];
unsigned jobsNum= 0;
int fg_ptr = -1;

unsigned childPid = 0;

sigset_t mask_one, prev_one, mask_all, prev_all;

void test_commandparser_testcase(int n, char casen[]);
void test_commandparser();
int commandparser(char *cmdline, char *argv[]);
void eval(char *cmdline);
int builtin_cmd(char *argv[]);

int addJob(const char *cmdline, pid_t pid);
void deleteJob(pid_t pid);
void printJobs();
void FrontgroundJob(const char *argv);
void BackgroundJob(const char *argv);

void sigchld_handler(int signo) {
    int olderrno = errno;
    pid_t pid;
    while ((pid=waitpid(-1, NULL, 0)) > 0) {
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        childPid = pid;
        deleteJob(childPid); 
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
        sio_puts("Hi\n");
    }
    errno = olderrno;
}

void send2fg_handler(int signo) {
    int olderrno = errno;
    if (fg_ptr < 0) exit(0);
    printf("TEST %d\n", fg_ptr);
    kill(jobs[fg_ptr].pid, signo);
    fg_ptr = -1;
    if (signo == SIGINT) {
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        jobs[fg_ptr].status = Deleted; 
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    errno = olderrno;
}

void test_main(){
    test_commandparser();
}

int main(){
    char cmdline[LINEMAX];
    sigemptyset(&mask_one);
    sigaddset(&mask_one, SIGCHLD);
    sigfillset(&mask_all);

    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, send2fg_handler);
    Signal(SIGTSTP, send2fg_handler);

    sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
    while (1) {
        printf(">");
        fgets(cmdline, LINEMAX, stdin);
        if (feof(stdin)) {
            exit(0);
        }
        eval(cmdline);
    }
    return 0;
} 

void eval(char *cmdline) {
    char *argv[MAXARGS];
    char buf[LINEMAX];

    strcpy(buf, cmdline);
    int bg = commandparser(buf, argv);
    printf("TEST bg=%d\n", bg);
    if (argv[0] == NULL) return;

    childPid = 0;
    if (builtin_cmd(argv) == 1) return;

    int pid = Fork();
    if (pid == 0) {
        if (execve(argv[0], argv, environ) < 0) {
            unix_error("command not found");
            exit(1);
        }
    }

    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    fg_ptr = addJob(cmdline, pid);
    sigprocmask(SIG_SETMASK, &prev_all, NULL);
    if (!bg) {
        printf("TEST childPid=%d, fg_ptr=%d\n", childPid, fg_ptr);
        while (childPid == 0 && fg_ptr != -1)
            sigsuspend(&prev_one);
        childPid = 0;
        fg_ptr = -1;
    }
    else fg_ptr = -1;
}

int commandparser(char *cmdline, char *argv[]) {
    int argc = 0, bg = 0;
    char *ptr;
    cmdline[strlen(cmdline)-1] = ' '; 
    while (*cmdline && *cmdline == ' ') cmdline ++;
    ptr = cmdline;
    argv[argc++] = ptr;
    while ((ptr=strchr(ptr, ' ')) != NULL) {
        *ptr++ = '\0';
        while (*ptr && *ptr == ' ') ptr ++;
        if (*ptr) argv[argc++] = ptr;
    }
    if (argc > 0 && strcmp(argv[argc-1], "&") == 0) {
        argv[argc-1] = NULL;
        bg = 1; 
    }else argv[argc++] = NULL;
    return bg;
}

int builtin_cmd(char *argv[]) {
    if (strcmp(argv[0], "jobs") == 0) 
        printJobs();
    else if (strcmp(argv[0], "fg") == 0) 
        FrontgroundJob(argv[1]);
    else if (strcmp(argv[0], "bg") == 0) 
        BackgroundJob(argv[1]);
    else if (strcmp(argv[0], "quit") == 0)
        exit(0);
    else if (strcmp(argv[0], "&") == 0)  ;
    else return 0;
    return 1;
}

void test_commandparser() {
    char case1[] = "pwd   \n";
    char case2[] = "  pwd\n";
    char case3[] = "  pwd  \n";
    char case4[] = "cd .\n";
    char case5[] = "  cd .\n";
    char case6[] = "cd   .\n";
    char case7[] = "    cd    .\n";
    char case8[] = "    cd    .   \n";

    test_commandparser_testcase(0, case1);
    test_commandparser_testcase(1, case2);
    test_commandparser_testcase(2, case3);
    test_commandparser_testcase(3, case4);
    test_commandparser_testcase(4, case5);
    test_commandparser_testcase(5, case6);
    test_commandparser_testcase(6, case7);
    test_commandparser_testcase(7, case8);
}
void test_commandparser_testcase(int n, char casen[]){
    char *argv[MAXARGS];
    printf("In case #%d: %s\n", n, casen);
    commandparser(casen, argv);
    for (int i=0;argv[i]!=NULL;i++){
        printf("%d: %s\n", i, argv[i]);
    }
    printf("\n");
}

int addJob(const char *cmdline, pid_t pid) {
    int i;
    for (i=0;i<jobsNum;i++)
        if (jobs[i].status == Deleted) break;
    if (i == MAXJOBS) unix_error("jobs full");
    jobs[i].pid = pid;
    strcpy(jobs[i].cmdline, cmdline);
    jobs[i].status = Running;
    if (i == jobsNum) jobsNum ++;
    return i;
}

void deleteJob(pid_t pid) {
    for (unsigned i=0;i<jobsNum;i++){
        if (jobs[i].pid == pid) {
            jobs[i].status = Deleted;
            return;
        }
    }
}

void printJobs() {
    for (unsigned i=0;i<jobsNum;i++) {
        if (jobs[i].status == Deleted) continue;
        printf("[%u] %u ", i, jobs[i].pid);
        switch(jobs[i].status) {
            case Running:printf("Running\t");break;
            case Stopped:printf("Stopped\t");break;
            default:assert(0);
        } 
        printf("%s\n", jobs[i].cmdline);
    }
}

void FrontgroundJob(const char *argv) {
    int id = argv!=NULL&&argv[0]=='%'?atoi(argv+1):atoi(argv);
    if (id < 0 || id > jobsNum) {
        fprintf(stderr, "no such job");
        return;
    }
    if (argv[0] == '%') {
        fg_ptr = id;
        kill(jobs[id].pid, SIGCONT);
        while (childPid == 0 && fg_ptr != -1)
            sigsuspend(&prev_one);
        childPid = 0;
        fg_ptr = -1;
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        jobs[id].status = Deleted;
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
    } else {
        unsigned i;
        for (i=0;i<jobsNum && jobs[i].pid!=id;i++)
        fg_ptr = i;        
        kill(id, SIGCONT);
        while (childPid == 0 && fg_ptr != -1)
            sigsuspend(&prev_one);
        childPid = 0;
        fg_ptr = -1;
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        jobs[id].status = Deleted;
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
}
void BackgroundJob(const char *argv) {
    int id = argv!=NULL&&argv[0]=='%'?atoi(argv+1):atoi(argv);
    if (id < 0 || id > jobsNum) {
        fprintf(stderr, "no such job");
        return;
    }
    if (argv[0] == '%') {
        kill(jobs[id].pid, SIGCONT);
    } else {
        kill(id, SIGCONT);
    }
}