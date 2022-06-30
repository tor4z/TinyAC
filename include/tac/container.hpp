#pragma once

#include <string>
namespace tac
{

class Container
{
public:
    Container();
    ~Container();
    
    void build();
    void run();
    void exec();
    void commit();
    void rm();
private:
    std::string containerID_;
};


void createParentProcess();
void containerInitProcess();
void commitContainer(const char *imageName);

}
