#include "tac/container.hpp"
#include "tac/log.hpp"
#include "tac/utils.hpp"
#include "tac/tar.hpp"
#include "tac/aufs.hpp"
#include "tac/tar.hpp"

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
        PERROR() << "remount root";
        return -1;
    }

    if (mount(newRoot, newRoot, NULL, MS_BIND, NULL) == -1)
    {
        PERROR() << "mount new root";
        return -1;;
    }

    snprintf(path, sizeof(path), "%s/%s", newRoot, oldRoot);
    if(Path::safe_mkdir(path, 0777) == -1)
    {
        PERROR() << "mkdir oldRoot";
        return -1;
    }

    if (pivot_root(newRoot, path) == -1)
    {
        PERROR() << "pivotRoot: " << newRoot << ";" << path;
        return -1;
    }

    if (chdir("/") == -1)
    {
        PERROR() << "chdir /";
        return -1;
    }

    if (umount2(oldRoot, MNT_DETACH) == -1)
    {
        PERROR() << "umount2";
        return -1;
    }

    if (Path::safe_rmdir(oldRoot) == -1)
    {
        PERROR() << "rmdir oldRoot";
        return -1;
    }
    return 0;
}


int setMount(const char *rootPath)
{
    if (pivotRoot(rootPath) == -1)
        return -1;

    if (Path::safe_mkdir("/proc", 0555) == -1)
    {
        PERROR() << "mkdir /proc";
        return -1;
    }

    if(mount(
        "proc", "/proc", "proc",
        MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL
    ) == -1)
    {
        PERROR() << "mount proc";
        return -1;
    }

    if (Path::safe_mkdir("/dev", 0755) == -1)
    {
        PERROR() << "mkdir /proc";
        return -1;
    }

    if (mount(
        "tmpfs", "/dev", "tmpfs",
        MS_NOSUID | MS_STRICTATIME, NULL
    ) == -1)
    {
        PERROR() << "mount dev";
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
        PERROR() << "unshare";
        return -1;
    }
    return 0;
}


void createParentProcess()
{
    DEBUG() << "set unshare";
    if (setUnshare() == -1)
        ERROR() << "set unshare error";

    pid_t child_pid = fork();
    if(child_pid == 0)
    {
        execl(SELF_EXE, SELF_EXE, "init", NULL);
        PFATAL() << "execl init";
    }
    else if (child_pid == -1)
    {
        PERROR() << "fork init";
    }

    waitpid(child_pid, NULL, 0);
}


void containerInitProcess()
{
    const char *rootPath = "/home/tor/var/tac/containers/con1/mnt";

    AUFS::makeSpace("/home/tor/var/tac/containers/con1");


    if (setMount(rootPath) == -1)
    {
        FATAL() << "set mount error";
    }

    pid_t child_pid = fork();
    if(child_pid == 0)
    {
        execl("/bin/sh", "/bin/sh", NULL);
        PFATAL() << "execl init process";
    }
    else if (child_pid == -1)
    {
        perror("fork");
        fprintf(stderr, "fork busybox error\n");
    }
    waitpid(child_pid, NULL, 0);
}


void commitContainer(const char *imageName)
{
    char imagePath[PATH_MAX];
    snprintf(
        imagePath, sizeof(imagePath),
        "%s/%s", "/home/tor/var/tac/images", imageName
    );
    if (tar(imagePath, "/home/tor/var/tac/containers/con1") == -1)
        FATAL() << "Commit container";
}


// void buildImage()
// {

// }

}