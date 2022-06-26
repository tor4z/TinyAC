#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>


#define CHILD_STACK_SIZE (1024 * 1024)


static int child_fn()
{
    const char *hostname = "newHostName";
    const char *cmd = "/bin/bash";

    sethostname(hostname, strlen(hostname));
    execl(cmd, cmd, NULL);
    perror("execl: ");
    return 1;
}


int main()
{
    char *child_stack;
    char *child_stack_top;

    child_stack = mmap(
        NULL, CHILD_STACK_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK,
        -1, 0
    );
    if (child_stack == MAP_FAILED)
    {
        fprintf(stderr, "mmap fail.\n");
        exit(1);
    }
    child_stack_top = child_stack + CHILD_STACK_SIZE;

    pid_t child_pid = clone(
        child_fn, child_stack_top,
        SIGCHLD | CLONE_NEWUTS | CLONE_NEWNET,
        NULL
    );
    if (child_pid == -1)
    {
        perror("clone: ");
        munmap(child_stack, CHILD_STACK_SIZE);
        exit(1);
    }

    waitpid(child_pid, NULL, 0);

    return 0;
}
