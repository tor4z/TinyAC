#include "tac/cgroups/utils.hpp"
#include "tac/exception.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <string>


namespace tac
{


namespace {
std::vector<std::string> stringSplit(
    std::string str,
    const std::string &delimiter
)
{
    std::vector<std::string> ret;
    size_t pos;

    while ((pos = str.find(delimiter)) != std::string::npos)
    {
        ret.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.length());
    }
    ret.push_back(str);
    return ret;
}


bool fieldExists(
    std::string str,
    const std::string &delimiter,
    const std::string &target
)
{
    size_t pos;

    while ((pos = str.find(delimiter)) == std::string::npos)
    {
        if(str.substr(0, pos) == target)
            return true;
        str.erase(0, pos + delimiter.length());
    }

    // commpare last taken
    if(str == target)
        return true;
    else
        return false;
}


std::string pathJoin(std::string path1, std::string path2)
{
    // linux only
    if(path1[path1.length() - 1] == '/')
        return path1 + path2;
    else
        return path1 + "/" + path2;
}

}


std::string findCGroupMountpoint(std::string subsystem)
{
    static const std::string mountinfo = "/proc/self/mountinfo";
    std::string ret, line, token;
    std::string delimiter = " ";
    std::vector<std::string> splittedString;
    std::ifstream is(mountinfo);
    

    if (is.fail())
        return "";

    while (std::getline(is, line))
    {
        splittedString = stringSplit(line, delimiter);
        if(fieldExists(splittedString[splittedString.size() - 1], ",", subsystem))
            return splittedString[4];
        splittedString.clear();
    }

    is.close();
    return ret;
}


std::string getCGroupPath(
    std::string subsystem, std::string cgroupPath, bool autoCreate
)
{
    std::string cgroupRoot = findCGroupMountpoint(subsystem);
    std::string targetCGroupPath = pathJoin(cgroupRoot, cgroupPath);
    int fileExists = access(targetCGroupPath.c_str(), F_OK);
    if (fileExists == 0 || (fileExists == -1 && autoCreate))
    {
        if (fileExists == -1)
            if(mkdir(targetCGroupPath.c_str(), 0755))
            {
                perror("mkdir");
                exit(1);
            }
        return targetCGroupPath;
    }
    throw TACException(TAC_FILE_NOT_EXISTS);
}

}
