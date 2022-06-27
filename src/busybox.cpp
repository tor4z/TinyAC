#include "busybox.hpp"
#include "utils.hpp"
#include <cstdio>
#include <cstdlib>
#include <linux/limits.h>
#include <sched.h>
#include <sys/stat.h>
#include <unistd.h>


namespace tac
{
int copyBusybox()
{
    // TODO fixme
    static const char *busyboxPath = "/usr/bin/busybox";
    char cwd[PATH_MAX - 10];
    char targetPath[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    snprintf(targetPath, sizeof(targetPath), "%s/%s", cwd, "busybox");

    if (copy(busyboxPath, targetPath, true) == -1)
    {
        fprintf(stderr, "copy error\n");
        return -1;
    }
    return 0;
}


int installBusybox()
{
    if (access("/bin", F_OK) == -1 && mkdir("/bin", 777) == -1)
    {
        perror("mkdir /bin");
        return -1;
    }

    pid_t child_pid = fork();
    if(child_pid == 0)
    {
        execl("/busybox", "/busybox", "--install", "/bin", NULL);
        perror("install busybox");
    }
    return 0;
}

}
