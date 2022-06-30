#pragma once

#include <string>
#include <vector>


namespace tac
{

class Document
{
public:
    enum DocType
    {
        Doc,
        Int,
        Str,
        List
    };

    Document();
    ~Document();
private:
    DocType type_;
    std::string key_;
    std::string str_;
    int int_;
    std::vector<Document> list_;
    Document doc_;
};


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
