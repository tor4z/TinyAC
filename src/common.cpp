#include "tac/common.hpp"
#include <string>
#include <map>


namespace tac
{

const std::map<const std::string, SubCommand> subCommandMap = {
    {"run", SubCommand::Run},
    {"exec", SubCommand::Exec},
    {"commit", SubCommand::Commit},
    {"build", SubCommand::Build},
    {"rm", SubCommand::Remove},
    {"kill", SubCommand::Kill},
    {"ls", SubCommand::List},
    {"images", SubCommand::Images},
    {"rmi", SubCommand::RemoveImg},
};

}
