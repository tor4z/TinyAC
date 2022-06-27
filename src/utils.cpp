#include "utils.hpp"
#include "tac/exception.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


namespace tac
{

int copy(const char *src, const char *dest, bool overWrite)
{
    int fd_src, fd_dest;
    int nread, nwrite, ret = 0;
    char buffer[4096];
    char *out_ptr;

    if (access(src, F_OK) == -1)
    {
        throw TACException(TAC_FILE_NOT_EXISTS);
    }

    if (access(dest, F_OK) == 0)
    {
        if (overWrite)
            remove(dest);
        else
            return 0;
    }

    fd_src = open(src, O_RDONLY);
    if (fd_dest == -1)
    {
        perror("copy open read");
        return -1;
    }
    fd_dest = open(dest, O_WRONLY | O_CREAT | O_EXCL, 0755);
    if (fd_dest == -1)
    {
        perror("copy open write");
        close(fd_src);
        return -1;
    }

    while ((nread = read(fd_src, buffer, sizeof(buffer))) > 0)
    {
        out_ptr = buffer;
        do
        {
            nwrite = write(fd_dest, out_ptr, nread);
            if (nwrite >= 0)
            {
                nread -= nwrite;
                out_ptr += nwrite;
            }
            else if (errno != EINTR)
            {
                perror("copy write dest");
                close(fd_src);
                close(fd_dest);
                return -1;
            }

            if (nread == 0)
            {
                break;
            }
        } while (nread > 0);
    }

    if (close(fd_src) == -1)
        ret = -1;
    if (close(fd_dest) == -1)
        ret = -1;
    return ret;
}


int copy(const std::string &src, const std::string &dest, bool overWrite)
{
    return copy(src.c_str(), dest.c_str(), overWrite);
}

}
