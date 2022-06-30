#pragma once

#include <string>
#include <vector>


namespace tac
{

struct Config
{
    std::string tacRoot;
    std::vector<std::string> images;
    std::vector<std::string> containers;

    void read();
    void write();
private:
    void parse();
};

}
