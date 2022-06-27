#pragma once

#include <string>


namespace tac
{

int copy(const char *src, const char *dest, bool overWrite);
int copy(const std::string &src, const std::string &dest, bool overWrite);

}
