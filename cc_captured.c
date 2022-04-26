#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <fcntl.h>

#define ARGLEN 256

volatile int die = 0;

struct args
{
    char cam[ARGLEN];
    char out[ARGLEN];
    char filename[ARGLEN];
};

void *th(void *param)
{
    struct args *args = (struct args *)param;
    while(1)
    {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char hn[128];
        gethostname(hn, 128);
        snprintf(args->filename, ARGLEN, "%s.%02d%02d%02d.%02d%02d%02d.%s.mp4\n",
                 hn,
                 tm.tm_year + 1900,
                 tm.tm_mon + 1,
                 tm.tm_mday,
                 tm.tm_hour,
                 tm.tm_min,
                 tm.tm_sec,
                 args->out);
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
            int r = prctl(PR_SET_PDEATHSIG, SIGINT);
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
                  args->out,
                  args->filename);
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
    printf("argv[1]:\n%s\n", av[1]);
    return 0;
    pthread_t t1, t2;
    struct args args1, args2;
    int r;
    if(signal(SIGINT, sighandler) == SIG_ERR)
    {
        printf("error installing signal handler\n");
        return 0;
    }
    snprintf(args1.cam, ARGLEN, "%s", "cam1");
    snprintf(args1.out, ARGLEN, "%s", "0");
    /* snprintf(args1.filename, ARGLEN, "%02d%02d%02d.%02d%02d%02d.0\n", */
    /*          tm.tm_year + 1900, */
    /*          tm.tm_mon + 1, */
    /*          tm.tm_mday, */
    /*          tm.tm_hour, */
    /*          tm.tm_min, */
    /*          tm.tm_sec); */

    snprintf(args2.cam, ARGLEN, "%s", "cam2");
    snprintf(args2.out, ARGLEN, "%s", "1");
    /* snprintf(args2.filename, ARGLEN, "%02d%02d%02d.%02d%02d%02d.1\n", */
    /*          tm.tm_year + 1900, */
    /*          tm.tm_mon + 1, */
    /*          tm.tm_mday, */
    /*          tm.tm_hour, */
    /*          tm.tm_min, */
    /*          tm.tm_sec); */

    pthread_create(&t1, NULL, th, (void *)&args1);
    pthread_create(&t2, NULL, th, (void *)&args2);

    while(die == 0)
    {
        sleep(1);
    }
    
    return 0;
}
