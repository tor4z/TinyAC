#include "tac/cli.hpp"
#include "tac/command.hpp"
#include "tac/version.hpp"
#include <iostream>

namespace tac
{

void processNoneSubCommand(Arguments &arg)
{
    if (arg.isSet(NONE_OPT_VERSION))
    {
        version();
        return;
    }

    usage();
}


void usage()
{
    std::cout << "usage " << std::endl;
    exit(0);
}


void version()
{
    std::cout << "Tiny Application Container (TAC) Version "
    << TAC_VERSION_MAJOR << "." << TAC_VERSION_MINOR
    << std::endl;
}


}
