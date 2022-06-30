#include "tac/container.hpp"
#include <cstring>


int main(int argc, const char *argv[])
{
    if (argc == 1)
        tac::createParentProcess();
    else
        if (std::strcmp(argv[1], "init") == 0)
            tac::containerInitProcess();
        else
            tac::commitContainer("con1.tacimg");
    return 0;
}
