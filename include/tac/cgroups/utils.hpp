#pragma once

#include <string>


namespace tac
{

std::string findCGroupMountpoint(std::string subsystem);
std::string getCGroupPath(
    std::string subsystem, std::string cgroupPath, bool autoCreate
);

}
