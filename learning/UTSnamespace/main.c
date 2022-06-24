#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/types.h>


#define CHILD_STACK_SIZE (1024 * 1024)


static void print_nodename()
{
    struct utsname utsname;
    uname(&utsname);
    printf("%s\n", utsname.nodename);
}


static int child_fn()
{
    printf("New UTS namespace nodename:");
    printf("Change nodename inside new UTS namespace\n");
    const char *hostname = "NewUTS";
    sethostname(hostname, strlen(hostname));

    printf("The new hostname is: ");
    print_nodename();

    execl("/bin/bash", "/bin/bash", NULL);
    return 0;
}


int main()
{
    char *child_stack;
    char *child_stack_top;

    child_stack = mmap(
        NULL, CHILD_STACK_SIZE, PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0
    );
    if (child_stack == MAP_FAILED)
        fprintf(stderr, "mmap failed.\n");
    child_stack_top = child_stack + CHILD_STACK_SIZE;

    printf("Original UTS namespace nodename: ");
    print_nodename();

    pid_t child_pid = clone(
        child_fn,
        child_stack_top,
        CLONE_NEWUTS | SIGCHLD,
        NULL
    );
    if (child_pid == -1)
    {
        fprintf(stderr, "clone failed (%d).\n", errno);
    }

    sleep(1);

    printf("Original UTS namespace nodename: ");
    print_nodename();

    waitpid(child_pid, NULL, 0);

    return 0;
}
