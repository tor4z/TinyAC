#include "common.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>


int copy_file(int infd, int outfd)
{
    assert(infd > 0 && outfd > 0);
    char buffer[BUFF_SIZE];
    char *write_ptr;
    size_t buffer_size = sizeof(buffer);
    int nread, nwrite;

    for (;;)
    {
        nread = read(infd, buffer, buffer_size);
        if (nread < 0)
            return -1;  // read fail
        if (nread == 0)
            return 0;   // read finished

        write_ptr = buffer;
        for (;;)
        {
            nwrite = write(outfd, buffer, nread);
            if (nwrite < 0)
                return -1;  // write fail

            nread -= nwrite;
            write_ptr += nwrite;

            if(errno == EINTR)
                return -1;

            // read next block
            if (nread == 0)
                break;
        }
    }
    return 0;
}


int process_file_or_dir(int outfd, const char *path)
{
    int infd;
    struct file_info file_info;
    struct stat file_stat;

    printf("packing file or dir: %s\n", path);
    stat(path, &file_stat);
    file_info.file_type = file_stat.st_mode & S_IFMT;
    memcpy(file_info.path, path, strlen(path) + 1);
    file_info.file_mode = file_stat.st_mode;
    file_info.finish_flag = 0;
    file_info.file_size = 0;

    if (file_info.file_type == S_IFDIR)
    {
        write_file_info(outfd, &file_info);
        return 0;
    }

    infd = open(path, O_RDONLY);
    if(infd == -1)
    {
        fprintf(stderr, "open %s file fail\n", path);
        return -1;
    }
    file_info.file_size = lseek(infd, 0, SEEK_END);
    write_file_info(outfd, &file_info);
    lseek(infd, 0, SEEK_SET);

    if (copy_file(infd, outfd) < 0)
    {
        close(infd);
        return -1;
    }
    close(infd);
    return 0;
}


int pack(int outfd, const char *path)
{
    DIR *dir;
    struct dirent *entry;
    char filename[MAX_PATH];

    if ((dir = opendir(path)) == NULL)
        return -1;

    if (process_file_or_dir(outfd, path) < 0)
        return -1;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(
            filename, sizeof(filename),
            "%s/%s", path, entry->d_name
        );

        if (entry->d_type == DT_DIR)
        {
            // if (process_file_or_dir(outfd, filename) < 0)
            //     return -1;
            if (pack(outfd, filename) < 0)
                return -1;
        }
        else
        {
            if (process_file_or_dir(outfd, filename) < 0)
                return -1;
        }
    }
    return 0;
}


int main()
{
    const char *in_dir = "./CMakeFiles.bak";
    const char *out_filename = "out.pkg";
    int outfd = open(
        out_filename, O_CREAT | O_RDWR, 0666
    );
    // lsdir(".", 0);
    pack(outfd, in_dir);

    struct file_info file_info;
    file_info.finish_flag = 1;
    file_info.file_mode = 0;
    file_info.file_size = 0;
    file_info.file_type = 0;
    memset(file_info.path, 0, MAX_PATH);
    write_file_info(outfd, &file_info);

    close(outfd);
    return 0;
}
