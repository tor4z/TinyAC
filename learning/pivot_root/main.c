#include <linux/limits.h>
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys//syscall.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/mman.h>


#define CHILD_STACK_SIZE (1024 * 1024)


static int pivot_root(const char *new_root, const char *old_root)
{
    return syscall(SYS_pivot_root, new_root, old_root);
}


static int child_fn(void *arg)
{
    char **args  = arg;
    char *new_root = args[0];
    const char *old_root = "/oldRootfs";
    char path[PATH_MAX];

    /* Ensure that 'new_root' and its parent mount don't have
        shared propagation (which would cause pivot_root() to
        return an error), and prevent propagation of mount
        events to the initial mount namespace. */

    /*
        none - This volume mount does not receive any subsequent mounts
        that are mounted to this volume or any of its subdirectories by
        the host. In similar fashion, no mounts created by the container
        are visible on the host. This is the default mode, and is equal
        to private mount propagation in Linux kernels.

        - A 'private mount' is defined as vfsmount that does not
		  receive or forward any propagation events.
    */
    if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1)
    {
        fprintf(stderr, "mount private fail\n");
        exit(1);
    }

    /* Ensure that 'new_root' is a mount point. */
    if (mount(new_root, new_root, NULL, MS_BIND, NULL) == -1)
    {
        fprintf(stderr, "mount bind fail\n");
        exit(1);
    }

    snprintf(path, sizeof(path), "%s/%s", new_root, old_root);
    if (mkdir(path, 0777) == -1)
    {
        fprintf(stderr, "mkdir fail\n");
        exit(1);
    }

    if (pivot_root(new_root, path) == -1)
    {
        fprintf(stderr, "pivot_root fail\n");
        exit(1);
    }

    if (chdir("/") == -1)
    {
        fprintf(stderr, "chdir fail\n");
        exit(1);
    }

    if (umount2(old_root, MNT_DETACH) == -1)
        perror("umount2");
    
    if (rmdir(old_root) == -1)
        perror("rmdir");
    
    execv(args[1], &args[1]);
    fprintf(stderr, "execv error\n");
    return 1;
}


int main(int argc, char *argv[])
{
    char *child_stack = mmap(
        NULL, CHILD_STACK_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK,
        -1, 0
    );
    if (child_stack == MAP_FAILED)
    {
        fprintf(stderr, "alloc stack failed.\n");
        return 1;
    }

    char *child_stack_top = child_stack + CHILD_STACK_SIZE;

    pid_t child_pid = clone(
        child_fn, child_stack_top, CLONE_NEWNS | SIGCHLD, &argv[1]
    );

    if (child_pid == -1)
    {
        fprintf(stderr, "clone failed\n");
        munmap(child_stack, CHILD_STACK_SIZE);
        return 1;
    }


    /*
    When a child process stops or terminates, SIGCHLD is sent to the parent process.
    The default response to the signal is to ignore it. The signal can be caught and
    the exit status from the child process can be obtained by immediately calling
    wait(2) and wait3(3C). This allows zombie process entries to be removed as quickly
    as possible. Example 4-2 demonstrates installing a handler that catches SIGCHLD.
    */
    if(wait(NULL) == -1)
    {
        fprintf(stderr, "wait fail\n");
        return 1;
    }

    return 0;
}
