// #define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>


int main()
{
    const char *hostname = "newHostname";

    if(unshare(CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS |
        CLONE_NEWNET | CLONE_NEWIPC) == -1)
    {
        perror("unshare:");
        exit(-1);
    }
    
    sethostname(hostname, strlen(hostname));
    execl("/bin/bash", "/bin/bash", NULL);
    perror("exec");
    return 0;
}
