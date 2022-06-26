#pragma once

#include "tac/cgroups/subsystem.hpp"
#include <sched.h>
#include <string>


namespace tac
{

class MemorySubsystem: public Subsystem
{
public:
    MemorySubsystem() = default;
    const std::string &name() const override;
    void set(const std::string &path, const ResourceConfig &res) override;
    void apply(const std::string &path, pid_t pid) override;
    void remove(const std::string &path) override;
private:
    static const std::string name_;
};

}
