#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <fcntl.h>

/* #define ARGLEN 256 */
#define PIPELINE_MAX_LEN 4096

int supply_date = 0;
volatile int die = 0;

/* struct args */
/* { */
/*     char cam[ARGLEN]; */
/*     char out[ARGLEN]; */
/*     char filename[ARGLEN]; */
/* }; */

/* struct args */
/* { */
/*     int ac; */
/*     char **av; */
/* }; */

void *th(void *param)
{
    /* struct args *args = (struct args *)param; */
    while(1)
    {
        char pipeline[PIPELINE_MAX_LEN];
        if(supply_date)
        {
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            char buf[128];
            snprintf(buf, 128, "%02d%02d%02d.%02d%02d%02d",
                     tm.tm_year + 1900,
                     tm.tm_mon + 1,
                     tm.tm_mday,
                     tm.tm_hour,
                     tm.tm_min,
                     tm.tm_sec);
            snprintf(pipeline, PIPELINE_MAX_LEN, (char *)param, buf);
        }
        else
        {
            strncpy(pipeline, (char *)param, PIPELINE_MAX_LEN);
        }

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
            /* int e; */
            wait(NULL);
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
            execl("/home/john/livefeed/cc_capture",
                  "/home/john/livefeed/cc_capture",
                  pipeline, (char *)NULL);
            /* args->cam, */
            /* args->out, */
            /* args->filename); */
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
    pthread_t t1;/* , t2; */
    /* struct args args1, args2; */
    /* int r; */

    char *pipeline = NULL;
    if(ac < 2)
    {
        
        fprintf(stderr, "%s: you must supply a pipeline\n",
            av[0]);
        return 0;
    }
    if(ac == 2)
    {
        pipeline = av[1];
    }
    if(ac == 3)
    {
        if(!strcmp(av[1], "-d"))
        {
            supply_date = 1;
            pipeline = av[2];
        }
        else
        {
            /* printusage(); */
            fprintf(stderr, "%s: unknown option %s\n",
                    av[0],
                    av[1]);
            return 0;
        }
    }
    /* struct args args = {ac, av}; */
    /* pthread_t t1, t2; */
    /* struct args args1, args2; */
    /* int r; */

    if(signal(SIGINT, sighandler) == SIG_ERR)
    {
        printf("error installing signal handler\n");
        return 0;
    }
    /* snprintf(args1.cam, ARGLEN, "%s", "cam1"); */
    /* snprintf(args1.out, ARGLEN, "%s", "0"); */
    /* snprintf(args1.filename, ARGLEN, "%02d%02d%02d.%02d%02d%02d.0\n", */
    /*          tm.tm_year + 1900, */
    /*          tm.tm_mon + 1, */
    /*          tm.tm_mday, */
    /*          tm.tm_hour, */
    /*          tm.tm_min, */
    /*          tm.tm_sec); */

    /* snprintf(args2.cam, ARGLEN, "%s", "cam2"); */
    /* snprintf(args2.out, ARGLEN, "%s", "1"); */
    /* snprintf(args2.filename, ARGLEN, "%02d%02d%02d.%02d%02d%02d.1\n", */
    /*          tm.tm_year + 1900, */
    /*          tm.tm_mon + 1, */
    /*          tm.tm_mday, */
    /*          tm.tm_hour, */
    /*          tm.tm_min, */
    /*          tm.tm_sec); */

    pthread_create(&t1, NULL, th, (void *)pipeline);
    /* pthread_create(&t2, NULL, th, (void *)&args2); */

    while(die == 0)
    {
        /* th((void *)pipeline); */
        sleep(1);
    }
    
    return 0;
}
