#pragma once


#include <exception>


namespace tac
{

#define EXCEPT_MAP(XX) \
    XX(SYS_ERROR, "System error") \
    XX(CLI_ARG_ERROR, "Cli argument error") \
    XX(FILE_NOT_EXISTS, "File not exists") \
    XX(SUBCOMMAND_UNKNOWN, "Sub command unkown") \
    XX(PARSE_CONFIG_ERROR, "Parse config error") \


enum TACError
{
#define MAKE_ENUM(e, s) TAC_##e,
    EXCEPT_MAP(MAKE_ENUM)
#undef MAKE_ENUM
};


class TACException: public std::exception
{
public:
    explicit TACException(TACError err);

    const char *what() const noexcept;
    TACError err () const;
private:
    TACError err_;
};

}
