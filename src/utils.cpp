#include "utils.hpp"
#include "tac/exception.hpp"
#include <ios>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sstream>
#include <random>


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

std::string genUUID()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    int i;

    ss << std::hex;
    for (i = 0; i < 8; i++)
        ss << dis(gen);

    for (i = 0; i < 8; i++)
        ss << dis(gen);

    ss << "4";
    for (i = 0; i < 3; i++)
        ss << dis(gen);

    ss << dis2(gen);
    for (i = 0; i < 3; i++)
        ss << dis(gen);

    for (i = 0; i < 12; i++)
        ss << dis(gen);

    return ss.str();
}

}
