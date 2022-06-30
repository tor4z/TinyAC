#pragma once

#include "tac/log.hpp"
#include <cstdio>
#include <cstring>
#include <string>


namespace tac
{

class Path
{
public:
    static int exists(const char *path);
    static int exists(const std::string &path);

    static int safe_mkdir(
        const char *path, __mode_t mode, bool force = false
    );
    static int safe_mkdir(
        const std::string &path, __mode_t mode, bool force = false
    );

    static int safe_rmdir(const char *path);
    static int safe_rmdir(const std::string &path);

    static int copy(
        const char *src, const char *dest, bool overWrite
    );
    static int copy(
        const std::string &src, const std::string &dest, bool overWrite
    );
};


std::string genUUID();

}
