#include "tac/cli.hpp"
#include "tac/common.hpp"
#include "tac/exception.hpp"
#include <cstdlib>
#include <iostream>


namespace tac
{

Cli::Cli(int argc, const char *argv[])
    : argc_(argc),
      argv_(argv),
      executableName_(argv[0]),
      subCommand_(SubCommand::None)
{}


SubCommand Cli::subCommand() const
{
    return subCommand_;
}


void Cli::parse()
{
    const char *curr;
    for (int i = 1; i < argc_; i++)
    {
        curr = argv_[i];

        if (curr[0] == '-')
        {
            // parse option
            switch (subCommand_)
            {
                case SubCommand::None:
                    parse_none_option(i);
                    break;
                case SubCommand::Run:
                    parse_run_option(i);
                    break;
                case SubCommand::Exec:
                    parse_exec_option(i);
                    break;
                case SubCommand::Commit:
                    parse_commit_option(i);
                    break;
                case SubCommand::Build:
                    parse_build_option(i);
                    break;
                case SubCommand::Remove:
                    parse_rm_option(i);
                    break;
                case SubCommand::Kill:
                    parse_kill_option(i);
                    break;
                case SubCommand::List:
                    parse_ls_option(i);
                    break;
                case SubCommand::Images:
                    parse_images_option(i);
                    break;
                case SubCommand::RemoveImg:
                    parse_rmi_option(i);
                    break;
                default:
                    throw TACException(TAC_SUBCOMMAND_UNKNOWN);
            }
        }
        else if (i == 1)
        {
            // sub command
            auto it = subCommandMap.find(curr);
            if (it == subCommandMap.end())
            {
                // Unknown sub command
                usage();
            }
            else
            {
                subCommand_ = it->second;
            }
        }
        else
        {
            // parse arguments
        }
    }
}


void Cli::parse_none_option(int argIndex)
{

}


void Cli::parse_run_option(int argIndex)
{

}


void Cli::parse_exec_option(int argIndex)
{

}


void Cli::parse_commit_option(int argIndex)
{

}


void Cli::parse_build_option(int argIndex)
{

}


void Cli::parse_rm_option(int argIndex)
{

}


void Cli::parse_kill_option(int argIndex)
{

}


void Cli::parse_ls_option(int argIndex)
{

}


void Cli::parse_images_option(int argIndex)
{

}


void Cli::parse_rmi_option(int argIndex)
{

}


void Cli::usage()
{
    std::cout << "usage " << std::endl;
    exit(0);
}

}
