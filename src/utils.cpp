#include "tac/utils.hpp"
#include "tac/exception.hpp"
#include "tac/log.hpp"
#include <ios>
#include <cstring>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sstream>
#include <random>


namespace tac
{

int Path::exists(const char *path)
{
    return access(path, F_OK);
}


int Path::exists(const std::string &path)
{
    return exists(path.c_str());
}


int Path::safe_mkdir(const char *path, __mode_t mode, bool force)
{
    if (exists(path) == 0)
    {
        if (force)
            rmdir(path);
        else
            return 0;
    }

    return mkdir(path, mode);
}


int Path::safe_mkdir(const std::string &path, __mode_t mode, bool force)
{
    return safe_mkdir(path.c_str(), mode, force);
}


int Path::safe_rmdir(const char *path)
{
    if (exists(path) == 0)
        return rmdir(path);
    return 0;
}


int Path::safe_rmdir(const std::string &path)
{
    return safe_rmdir(path.c_str());
}


int Path::copy(const char *src, const char *dest, bool overWrite)
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


int Path::copy(const std::string &src, const std::string &dest, bool overWrite)
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
