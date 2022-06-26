#include "tac/cgroups/cpu.hpp"
#include <string>


namespace tac
{

const std::string CPUSubsystem::name_ = "cpu";


const std::string &CPUSubsystem::name() const
{
    return name_;
}


void CPUSubsystem::set(
    const std::string &path, const ResourceConfig &res
)
{

}


void CPUSubsystem::apply(const std::string &path, pid_t pid)
{

}


void CPUSubsystem::remove(const std::string &path)
{
    
}


}
