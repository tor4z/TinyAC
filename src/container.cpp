#include "tac/container.hpp"
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mount.h>


#define SELF_EXE "/proc/self/exe"


namespace tac
{

void setMount()
{
    if(mount(
        "proc", "/proc", "proc",
        MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL
    ) == -1)
    {
        perror("mount proc");
        exit(1);
    }

    if (mount(
        "tmpfs", "/dev", "tmpfs",
        MS_NOSUID | MS_STRICTATIME, NULL
    ) == -1)
    {
        perror("mount dev");
        exit(1);
    }
}


void createParentProcess()
{
    if(unshare(
        CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS |
        CLONE_NEWNET | CLONE_NEWIPC
    ) == -1)
    {
        perror("unshare");
        exit(1);
    }

    pid_t child_pid = fork();
    if(child_pid == 0)
    {
        execl(SELF_EXE, SELF_EXE, "init", NULL);
    }
    else
    {
        waitpid(child_pid, NULL, 0);
        printf("parent\n");
    }
    // Should be never reach hear
    perror("execl");
    exit(1);
}


void containerInitProcess()
{
    setMount();
    execl("/bin/bash", "/bin/bash", NULL);
}

}