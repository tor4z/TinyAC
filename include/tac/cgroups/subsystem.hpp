#pragma once

#include <sched.h>
#include <string>


struct ResourceConfig
{
    std::string memoryLimit;
    std::string CPUShare;
    std::string CPUSet;
};


class Subsystem
{
public:
    virtual const std::string &name() const = 0;
    virtual void set(const std::string &path, const ResourceConfig &res) = 0;
    virtual void apply(const std::string &path, pid_t pid) = 0;
    virtual void remove(const std::string &path) = 0;
};

