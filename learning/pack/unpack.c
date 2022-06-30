#include "common.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>


int safe_mkdir(const char *path, uint32_t file_mode)
{
    if (access(path, F_OK) == -1)
    {
        printf("mkdir %s\n", path);
        return mkdir(path, file_mode);
    }
    return 0;
}


int write_n_byte_file(int infd, int outfd, int64_t n)
{
    int buff_size;
    int nread, nwrite;
    char *write_ptr;
    char buffer[BUFF_SIZE];

    if(n > BUFF_SIZE)
        buff_size = BUFF_SIZE;
    else
        buff_size = n;

    for (;;)
    {
        nread = read(infd, buffer, buff_size);
        if (nread < 0) // read fail
            return -1;

        // printf("byte to read %ld, we read: %d, buffer_size %d\n", n, nread, buff_size);
        if (nread == 0)
        {
            perror("read");
            if (n == 0)     // read finished, never reach here
                return 0;
            else
                return -1;  // some bytes not read yet, but we cannot read
        }

        // set write buffer ptr
        write_ptr = buffer;
        // update buffer size
        n -= nread;
        if(n < buff_size)
            buff_size = n;

        for (;;)
        {
            nwrite = write(outfd, write_ptr, nread);
            if (nwrite < 0)
            {
                perror("write");
                return -1;  // write fail
            }
            if (nwrite == 0 && nread == 0)
                break;

            nread -= nwrite;
            write_ptr += nwrite;

            if (errno == EINTR)
            {
                perror("error");
                return -1;  // write error
            }

            if (nread == 0)
                break;      // write finished
        }

        if (n == 0)
            return 0;       // copy finished
    }

    return 0;
}



int unpack(int infd)
{
    int outfd;
    int res;
    struct file_info file_info;

    for(;;)
    {
        // check finished
        if(read_file_info(infd, &file_info) < 0)
            return -1;

        printf(
            "unpacking file or dir: %s, %u\n",
            file_info.path, file_info.finish_flag
        );
        if (file_info.finish_flag == 1)
            return 0;

        if (file_info.file_type == S_IFDIR)
        {
            if (safe_mkdir(file_info.path, file_info.file_mode) < 0)
                return -1;
            continue;
        }

        outfd = open(
            file_info.path, O_CREAT | O_RDWR,
            file_info.file_mode
        );
        if (outfd < 0)
        {
            perror("open file");
            return -1;
        }
        if (write_n_byte_file(infd, outfd, file_info.file_size) < 0)
        {
            fprintf(stderr, "write file fail.\n");
            return -1;
        }
        close(outfd);
    }

    // never reach here
    return 0;
}


int main()
{
    const char *filename  = "out.pkg";
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open file");
        fprintf(stderr, "fail to open file\n");
        return 1;
    }

    if (unpack(fd) < 0)
    {
        fprintf(stderr, "unpack fail\n");
    }
    else
    {
        printf("unpack success\n");
    }
    close(fd);
    return 0;
}
