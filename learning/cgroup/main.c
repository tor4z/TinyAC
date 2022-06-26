#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/mman.h>


#define CGROUP_MEMORY_HIERARCHY_MOUNT "/sys/fs/cgroup/memory"
#define TEST_MEMORY_LIMIT "test_memory_limit"
#define SELF_EXE "/proc/self/exe"
#define CHILD_STACK_SIZE (1024 * 1024)


static int child_fn()
{
    printf("start exec ...\n");
    execl(SELF_EXE, SELF_EXE, NULL);
    return 1;
}


int main(int argc, char * argv[])
{
    printf("running %s\n", argv[0]);

    if(strcmp(argv[0], SELF_EXE) == 0)
    {
        // the container process
        printf("In container process Current PID: %d\n", getpid());
        execl(
            "/bin/bash", "/bin/bash",
            "-c", "stress --vm-bytes 200m --vm-keep -m 1",
            NULL
        );
        perror("exec: ");
        return 1;
    }
    else
    {
        char *child_stack;
        char *child_stack_top;
        
        child_stack = mmap(
            NULL, CHILD_STACK_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_STACK | MAP_ANONYMOUS | MAP_PRIVATE,
            -1, 0
        );
        if (child_stack == MAP_FAILED)
        {
            perror("mmap: ");
            exit(1);
        }
        child_stack_top = child_stack + CHILD_STACK_SIZE;

        pid_t child_pid = clone(
            child_fn, child_stack_top,
            SIGCHLD | CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS,
            NULL
        );

        if (child_pid == -1)
        {
            perror("clone: ");
            munmap(child_stack, CHILD_STACK_SIZE);
            exit(1);
        }

        printf("child PID: %d\n", child_pid);
        char memory_hierarchy[64];
        char memory_hierarchy_tasks[128];
        char memory_hierarchy_mem_limit[128];
        snprintf(
            memory_hierarchy, sizeof(memory_hierarchy),
            "%s/%s", CGROUP_MEMORY_HIERARCHY_MOUNT, TEST_MEMORY_LIMIT
        );
        snprintf(
            memory_hierarchy_tasks, sizeof(memory_hierarchy_tasks),
            "%s/%s", memory_hierarchy, "tasks"
        );
        snprintf(
            memory_hierarchy_mem_limit, sizeof(memory_hierarchy_mem_limit),
            "%s/%s", memory_hierarchy, "memory.limit_in_bytes"
        );

        if (access(memory_hierarchy, F_OK) == -1)
        {
            if (mkdir(memory_hierarchy, 0755) == -1)
            {
                perror("mkdir: ");
                exit(1);
            }
        }

        int fd = open(memory_hierarchy_tasks, O_WRONLY | O_APPEND, 0644);
        if (fd == -1)
        {
            perror("open tasks: ");
            exit(1);
        }
        char pid_str[16];
        snprintf(pid_str, 16, "%d\n", child_pid);
        if (write(fd, pid_str, strlen(pid_str)) == -1)
        {
            perror("write tasks: ");
            exit(1);
        }
        close(fd);

        // fd = open(memory_hierarchy_tasks, O_RDONLY);
        // char buff[512];
        // read(fd, buff, 512);
        // close(fd);
        // printf("read tasks: %s\n", buff);

        fd = open(memory_hierarchy_mem_limit, O_WRONLY | O_APPEND, 0644);
        if (fd == -1)
        {
            fprintf(stderr, "open mem limit: \n");
            exit(1);
        }
        char mem_limit[16];
        snprintf(mem_limit, 16, "100m\n");
        if (write(fd, mem_limit, strlen(mem_limit)) == -1)
        {
            fprintf(stderr, "write mem limit: \n");
            exit(1);
        }
        close(fd);


        waitpid(child_pid, NULL, 0);
    }

    return 0;
}
