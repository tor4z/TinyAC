#include "tac/container.hpp"
#include "tac/log.hpp"
#include "busybox.hpp"
#include "utils.hpp"
#include <cstddef>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/syscall.h>


#define SELF_EXE "/proc/self/exe"


namespace tac
{


namespace
{
static int pivot_root(const char *new_root, const char *old_root)
{
    return syscall(SYS_pivot_root, new_root, old_root);
}
}



int pivotRoot(const char *newRoot)
{
    const char *oldRoot = "oldRoot";
    char path[PATH_MAX];

    if(mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1)
    {
        perror("remount root");
        return -1;
    }

    if (mount(newRoot, newRoot, NULL, MS_BIND, NULL) == -1)
    {
        perror("mount new root");
        return -1;;
    }

    snprintf(path, sizeof(path), "%s/%s", newRoot, oldRoot);
    if(access(path, F_OK) == -1)
    {
        if (mkdir(path, 0777) == -1)
        {
            perror("mkdir oldRoot");
            return -1;
        }
    }

    if (pivot_root(newRoot, path) == -1)
    {
        perror("pivotRoot");
        return -1;
    }

    if (chdir("/") == -1)
    {
        perror("chdir /");
        return -1;
    }

    if (umount2(oldRoot, MNT_DETACH))
    {
        perror("umount2");
        return -1;
    }

    if (rmdir(oldRoot) == -1)
    {
        perror("rmdir oldRoot");
        return -1;
    }
    return 0;
}


int setMount()
{
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        return -1;
    }
    if (pivotRoot(cwd) == -1)
        return -1;

    if (access("/proc", F_OK) == -1 &&
        mkdir("/proc", 0555) == -1)
    {
        perror("mkdir /proc");
        return -1;
    }

    if(mount(
        "proc", "/proc", "proc",
        MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL
    ) == -1)
    {
        perror("mount proc");
        return -1;
    }

    if (access("/dev", F_OK) == -1 && 
        mkdir("/dev", 0755) == -1)
    {
        perror("mkdir /proc");
        return -1;
    }

    if (mount(
        "tmpfs", "/dev", "tmpfs",
        MS_NOSUID | MS_STRICTATIME, NULL
    ) == -1)
    {
        perror("mount dev");
        return -1;
    }
    return 0;
}


int setUnshare()
{
    if(unshare(
        CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS |
        CLONE_NEWNET | CLONE_NEWIPC
    ) == -1)
    {
        perror("unshare");
        return -1;
    }
    return 0;
}


void createParentProcess()
{
    DEBUG() << "set unshare";
    if (setUnshare() == -1)
    {
        fprintf(stderr, "set unshare error\n");
    }

    pid_t child_pid = fork();
    if(child_pid == 0)
    {
        execl(SELF_EXE, SELF_EXE, "init", NULL);
        perror("execl init");
        exit(1);
    }
    else if (child_pid == -1)
    {
        perror("fork");
        fprintf(stderr, "fork init error\n");
    }

    waitpid(child_pid, NULL, 0);
    // Should be never reach hear
    printf("exit\n");
}


void containerInitProcess()
{
    printf("copy busybox\n");
    if (copyBusybox() == -1)
    {
        fprintf(stderr, "copy busybox error\n");
    }

    printf("set mount\n");
    if (setMount() == -1)
    {
        fprintf(stderr, "set mount error\n");
    }

    printf("installing busybox\n");
    if (installBusybox() == -1)
    {
        fprintf(stderr, "install busybox error\n");
    }

    printf("run busybox\n");
    pid_t child_pid = fork();
    if(child_pid == 0)
    {
        execl("/busybox", "/busybox", "sh", NULL);
        perror("execl init process");
        exit(1);
    }
    else if (child_pid == -1)
    {
        perror("fork");
        fprintf(stderr, "fork busybox error\n");
    }
    waitpid(child_pid, NULL, 0);
    printf("child process exit.\n");
}

}