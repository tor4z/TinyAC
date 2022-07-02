#include "tac/cli.hpp"
#include "tac/common.hpp"
#include "tac/exception.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string_view>
#include <vector>


namespace tac
{

const std::vector<OptConfig> ArgumentParser::noneOptConfigs = {
    {NONE_OPT_VERSION, 'v', "version", false},
    {NONE_OPT_HELP, 'h', "help", false}
};


ArgumentParser::ArgumentParser(
    int argc, const char **argv, SubCommand subCommand
): argc_(argc), argv_(argv), subCommand_(subCommand)
{}


bool Arguments::isSet(uint32_t mask) const
{
    return (opts_ & mask) > 0;
}


void Arguments::set(uint32_t mask)
{
    opts_ |= mask;
}


int Arguments::argc() const
{
    return argc_;
}


const char **Arguments::argv() const
{
    return argv_;
}


SubCommand Arguments::subCommand() const
{
    return subCommand_;
}

const std::vector<const char*> Arguments::getOptArgs(uint32_t mask) const
{
    auto it = optArgsMap_.find(mask);
    if (it == optArgsMap_.end())
    {
        throw TACException(TAC_CLI_ARG_ERROR);
    }
    return it->second;
}


Arguments ArgumentParser::parse()
{
    Arguments args;

    if (argc_ == 0)
        return args;

    CharStream cs;
    const OptConfig *optConfig;
    while (argc_ > 0)
    {
        if (!isAnOption(*argv_))
            break;

        --argc_;
        cs.reset(*(argv_++));
        cs.skipWhiteSpace();
        if (cs.next() == '-')
        {
            if (cs.peek() == '-')
            {
                // long option
                cs.next();  // skip the second '-'
                optConfig = findOptConfig(
                    currOptConfigs(),
                    cs.token()
                );
                // set option
                args.set(optConfig->mask);
            }
            else
            {
                // short option
                cs.skipWhiteSpace();
                while (!cs.isWhiteSpace())
                {
                    optConfig = findOptConfig(
                        currOptConfigs(),
                        cs.next()
                    );
                    args.set(optConfig->mask);
                }
            }
        }

        if (optConfig->withArg)
        {
            std::vector<const char*> argList;
            while (!isAnOption(*argv_))
            {
                argList.push_back(*argv_);
                ++argv_;
                --argc_;
            }
            args.optArgsMap_.emplace(optConfig->mask, argList);
        }
    }

    return args;
}


const OptConfig *ArgumentParser::findOptConfig(
    const std::vector<OptConfig> &opts, char opt
)
{
    for (auto &it: opts)
    {
        if (it.shortName == opt)
            return &it;
    }
    throw TACException(TAC_CLI_ARG_ERROR);
}


const OptConfig *ArgumentParser::findOptConfig(
    const std::vector<OptConfig> &opts, const std::string_view &opt
)
{
    for (auto &it: opts)
    {
        if (opt.compare(it.longName) == 0)
            return &it;
    }
    throw TACException(TAC_CLI_ARG_ERROR);
}


const OptConfig *ArgumentParser::findOptConfig(
    const std::vector<OptConfig> &opts, const char *opt
)
{
    return findOptConfig(opts, opt);
}


bool ArgumentParser::isAnOption(const char *arg)
{
    return arg[0] == '-';
}

const std::vector<OptConfig> &ArgumentParser::currOptConfigs()
{
    if (subCommand_ == SubCommand::None)
        return noneOptConfigs;
    if (subCommand_ == SubCommand::Run)
        return runOptConfigs;
    if (subCommand_ == SubCommand::Exec)
        return execOptConfigs;
    if (subCommand_ == SubCommand::Commit)
        return commitOptConfigs;
    if (subCommand_ == SubCommand::Build)
        return buildOptConfigs;
    if (subCommand_ == SubCommand::Remove)
        return removeOptConfigs;
    if (subCommand_ == SubCommand::Kill)
        return killOptConfigs;
    if (subCommand_ == SubCommand::List)
        return listOptConfigs;
    if (subCommand_ == SubCommand::RemoveImg)
        return removeImgOptConfigs;
    if (subCommand_ == SubCommand::Images)
        return imagesOptConfigs;

    // never reach here
    throw TACException(TAC_CLI_ARG_ERROR);
}


ArgumentParser::CharStream::CharStream(const char *arg)
    : dataStr_(arg)
{
    iter_ = dataStr_.begin();
}


char ArgumentParser::CharStream::peek()
{
    if (hasNext())
        return *iter_;
    else
        return '\0';
}


char ArgumentParser::CharStream::next()
{
    char ch;
    if (hasNext())
    {
        ch = *iter_;
        ++iter_;
    }
    else
    {
        ch = '\0';
    }

    return ch;
}


bool ArgumentParser::CharStream::hasNext() const
{
    if (iter_ == dataStr_.end())
        return true;
    return false;
}


void ArgumentParser::CharStream::reset(const char *arg)
{
    dataStr_ = arg;
    iter_ = dataStr_.begin();
}


void ArgumentParser::CharStream::skipWhiteSpace()
{
    while (hasNext())
    {
        if (isWhiteSpace())
            ++iter_;
        else
            break;
    }
}


bool ArgumentParser::CharStream::isWhiteSpace()
{
    if (*iter_ == ' ' || *iter_ == '\t' || *iter_ == '\n')
        return true;
    return false;
}


const std::string_view ArgumentParser::CharStream::token()
{
    skipWhiteSpace();
    const char *strBegin = &(*iter_);

    while (hasNext() && !isWhiteSpace())
    {
        next();
    }

    return std::string_view(strBegin, &(*iter_) - strBegin);
}


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


Arguments Cli::parse()
{
    Arguments emptyArg;
    if(argc_ < 2)
        return emptyArg;

    const char *firstArg = argv_[1];
    if (ArgumentParser::isAnOption(firstArg))
    {
        subCommand_ = SubCommand::None;
    }
    else
    {
        auto it = subCommandMap.find(firstArg);
        if (it == subCommandMap.end())
        {
            // Unknown sub command
            return emptyArg;
        }
        subCommand_ = it->second;

        // skip executable name and sub-command
        argc_ -= 2;
        argv_ += 2;
    }

    ArgumentParser argParser(argc_, argv_, subCommand_);
    return argParser.parse();
}


}
