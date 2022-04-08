#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <fcntl.h>

#define ARGLEN 64

volatile int die = 0;

struct args
{
    char cam[ARGLEN];
    char out[ARGLEN];
};

void *th(void *param)
{
    struct args *args = (struct args *)param;
    while(1)
    {
        printf("thread %s: forking\n", args->out);
        pid_t ppid_before_fork = getpid();
        pid_t pid = fork();

        if(pid < 0)
        {
            perror(0);
            exit(1);
        }
        else if(pid)
        {
            /* parent */
            int e;
            printf("thread %s: waiting\n", args->out);
            wait(NULL);
            printf("thread %s: child process died, restarting...\n", args->out);
        }
        else
        {
            /* child */
            int r = prctl(PR_SET_PDEATHSIG, SIGHUP);
            if(r < 0)
            {
                perror(0);
                exit(1);
            }
            if(getppid() != ppid_before_fork)
            {
                exit(1);
            }
            printf("thread %s: child process execing cc_capture\n", args->out);
            execl("/home/john/livefeed/cc_capture",
                  "/home/john/livefeed/cc_capture",
                  args->cam,
                  args->out);
        }
    }
}

void sighandler(int signo)
{
    if(signo == SIGINT)
    {
        die = 1;
    }       
}

int main(int ac, char **av)
{
    pthread_t t1, t2;
    struct args args1, args2;
    int r;
    if(signal(SIGINT, sighandler) == SIG_ERR)
    {
        printf("error installing signal handler\n");
        return 0;
    }
    /* int lockfd = open("/home/john/cc_capture_lock", O_RDRW | O_CREAT | O_EXCL | O_NOFOLLOW); */
    /* if(lockfd < 0) */
    /* { */
    /*     switch(errno) */
    /*     { */
    /*     case EEXIST: */

    /*         break; */
    /*     default: */
    /*         perror("failed to open lock file. "); */
    /*         break; */
    /*     } */
    /* } */
    /* else */
    /* { */
    /*     flock(lockfd, LOCK_EX); */
    /* } */
    snprintf(args1.cam, ARGLEN, "%s", "cam1");
    snprintf(args1.out, ARGLEN, "%s", "0");
    snprintf(args2.cam, ARGLEN, "%s", "cam2");
    snprintf(args2.out, ARGLEN, "%s", "1");

    pthread_create(&t1, NULL, th, (void *)&args1);
    pthread_create(&t2, NULL, th, (void *)&args2);

    while(die == 0)
    {
        sleep(1);
    }
    
    return 0;
}
