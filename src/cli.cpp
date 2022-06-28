#include "cli.hpp"
#include "common.hpp"


namespace tac
{

Cli::Cli(int argc, const char *argv[]): argc_(argc), argv_(argv)
{}


SubCommand Cli::subCommand() const
{
    return subCommand_;
}


void Cli::parse()
{

}


void Cli::usage()
{
    
}

}
