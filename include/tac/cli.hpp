#pragma once

#include "common.hpp"

namespace tac
{

class Cli
{
public:
    Cli(int argc, const char *argv[]);
    SubCommand subCommand() const;

private:
    const int argc_;
    const char **argv_;
    SubCommand subCommand_;

    void parse();
    void usage();
};

}
