#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/types.h>


#define CHILD_STACK_SIZE (1024 * 1024)


static int child_fn()
{
    const char * cmd = "/bin/bash";
    const char *hostname = "newHostName";

    sethostname(hostname, strlen(hostname));
    execl(cmd, cmd, NULL);
    perror("execl");
    return 1;
}


int main()
{
    char *child_stack;
    char *child_stack_top;

    child_stack = mmap(
        NULL, CHILD_STACK_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_STACK | MAP_ANONYMOUS,
        -1, 0
    );
    if (child_stack == MAP_FAILED)
    {
        fprintf(stderr, "alloc child stack fail.\n");
        exit(1);
    }
    child_stack_top = child_stack + CHILD_STACK_SIZE;

    pid_t child_pid = clone(
        child_fn, child_stack_top,
        CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD,
        NULL
    );
    if (child_pid == -1)
    {
        perror("clone fail: ");
        munmap(child_stack, CHILD_STACK_SIZE);
        exit(1);
    }

    waitpid(child_pid, NULL, 0);

    return 0;
}
