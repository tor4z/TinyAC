#include "tac/common.hpp"
#include "tac/cli.hpp"
#include "tac/command.hpp"
#include "tac/container.hpp"
#include <cstring>


int main(int argc, const char *argv[])
{
    tac::Cli cli(argc, argv);
    tac::Arguments arg = cli.parse();

    switch (arg.subCommand())
    {
        case tac::SubCommand::None:
            tac::processNoneSubCommand(arg);
            break;
        case tac::SubCommand::Run:
            tac::processRunSubCommand(arg);
            break;
        case tac::SubCommand::Exec:
            tac::processExecSubCommand(arg);
            break;
        case tac::SubCommand::Commit:
            tac::processCommitSubCommand(arg);
            break;
        case tac::SubCommand::Build:
            tac::processBuildSubCommand(arg);
            break;
        case tac::SubCommand::Remove:
            tac::processRemoveSubCommand(arg);
            break;
        case tac::SubCommand::Kill:
            tac::processKillSubCommand(arg);
            break;
        case tac::SubCommand::List:
            tac::processListSubCommand(arg);
            break;
        case tac::SubCommand::Images:
            tac::processImagesSubCommand(arg);
            break;
        case tac::SubCommand::RemoveImg:
            tac::processRemoveImgSubCommand(arg);
            break;
        default:
            tac::usage();
    }

    // if (argc == 1)
    //     tac::createParentProcess();
    // else
    //     if (std::strcmp(argv[1], "init") == 0)
    //         tac::containerInitProcess();
    //     else
    //         tac::commitContainer("con1.tacimg");
    return 0;
}
