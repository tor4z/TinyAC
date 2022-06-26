// #define _GNU_SOURCE
#include "tac/container.hpp"


int main(int argc, const char *argv[])
{
    if (argc == 1)
        tac::createParentProcess();
    else
        tac::containerInitProcess();
    return 0;
}
