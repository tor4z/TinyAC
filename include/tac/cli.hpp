#pragma once

#include "common.hpp"
#include "tac/config.hpp"
#include <cstdint>
#include <map>
#include <string_view>
#include <vector>
#include <string>


namespace tac
{

#define NONE_OPT_VERSION 0x00000001
#define NONE_OPT_HELP 0x00000002


struct OptConfig
{
    uint32_t mask;
    const char shortName = '\0';
    const char *longName;
    bool withArg;
};


class Arguments
{
public:
    Arguments() = default;

    bool isSet(uint32_t mask) const;
    const std::vector<const char*> getOptArgs(uint32_t mask) const;
    const char **argv() const;
    int argc() const;
    SubCommand subCommand() const;

private:
    int argc_;
    const char **argv_;
    std::map<uint32_t, const std::vector<const char*>> optArgsMap_;
    SubCommand subCommand_;
    uint32_t opts_;

    void set(uint32_t mask);
    friend class ArgumentParser;
};


class ArgumentParser
{
public:
    ArgumentParser(int argc, const char **argv, SubCommand subCommand);

    // bool isSet(uint32_t mask) const;
    // void set(uint32_t mask);
    // const std::vector<const char*> getOptArgs(uint32_t mask) const;
    // const char **argv() const;
    // int argc() const;
    Arguments parse();

    static bool isAnOption(const char *arg);
private:
    class CharStream
    {
    public:
        CharStream(const char *arg);
        CharStream() = default;
        char peek();
        char next();
        bool hasNext() const;
        void reset(const char *arg);
        void skipWhiteSpace();
        bool isWhiteSpace();

        const std::string_view token();
    private:
        using Iter = std::string_view::iterator;
        Iter iter_;
        std::string_view dataStr_;
    };

    int argc_;
    const char **argv_;
    SubCommand subCommand_;
    // std::map<uint32_t, const std::vector<const char*>> optArgsMap_;
    // uint32_t opts_;

    static const std::vector<OptConfig> noneOptConfigs;
    static const std::vector<OptConfig> runOptConfigs;
    static const std::vector<OptConfig> execOptConfigs;
    static const std::vector<OptConfig> commitOptConfigs;
    static const std::vector<OptConfig> buildOptConfigs;
    static const std::vector<OptConfig> removeOptConfigs;
    static const std::vector<OptConfig> killOptConfigs;
    static const std::vector<OptConfig> listOptConfigs;
    static const std::vector<OptConfig> removeImgOptConfigs;
    static const std::vector<OptConfig> imagesOptConfigs;

    static const OptConfig *findOptConfig(
        const std::vector<OptConfig> &opts, char opt
    );
    static const OptConfig *findOptConfig(
        const std::vector<OptConfig> &opts,
        const char *opt
    );
    static const OptConfig *findOptConfig(
        const std::vector<OptConfig> &opts,
        const std::string_view &opt
    );

    const std::vector<OptConfig> &currOptConfigs();
};



class Cli
{
public:
    Cli(int argc, const char *argv[]);
    SubCommand subCommand() const;
    Arguments parse();

private:
    int argc_;
    const char **argv_;
    const char *executableName_;
    SubCommand subCommand_;
    const char **args_;
};

}
