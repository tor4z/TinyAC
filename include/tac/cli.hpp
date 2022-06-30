#pragma once

#include "common.hpp"
#include <cstdint>

namespace tac
{


struct Options
{
    uint64_t noneOpts = 0;
    uint64_t runOpts = 0;
    uint64_t execOpts = 0;
    uint64_t commitOpts = 0;
    uint64_t buildOpts = 0;
    uint64_t rmOpts = 0;
    uint64_t killOpts = 0;
    uint64_t lsOpts = 0;
    uint64_t imagesOpts = 0;
    uint64_t rmiOpts = 0;
};


enum class NoneOpts
{
    Version = 1,
    Help = 2
};


class Cli
{
public:
    Cli(int argc, const char *argv[]);
    SubCommand subCommand() const;

private:
    const int argc_;
    const char **argv_;
    const char *executableName_;
    SubCommand subCommand_;
    Options options_;

    void parse();
    void usage();

    // for no subcommand
    void parse_none_option(int argIndex);
    void parse_run_option(int argIndex);
    void parse_exec_option(int argIndex);
    void parse_commit_option(int argIndex);
    void parse_build_option(int argIndex);
    void parse_rm_option(int argIndex);
    void parse_kill_option(int argIndex);
    void parse_ls_option(int argIndex);
    void parse_images_option(int argIndex);
    void parse_rmi_option(int argIndex);
};

}
