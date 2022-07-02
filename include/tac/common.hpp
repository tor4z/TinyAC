#pragma once

#include <map>
#include <string>


namespace tac
{

enum class SubCommand
{
    None,       // no sub comment, for show help or version message
    Run,        // create and run a conntainer
    Exec,       // run a exist container
    Commit,     // commit a container as image
    Build,      // build a image from config
    Remove,     // remove a container
    Kill,       // kill a container
    List,       // list the containers
    Images,     // list images
    RemoveImg   // remove a image
};


extern const std::map<const std::string, SubCommand> subCommandMap;

}

