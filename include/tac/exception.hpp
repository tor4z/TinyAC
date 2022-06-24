#pragma once


#include <exception>


namespace tac
{

#define EXCEPT_MAP(XX) \
    XX(E, "e") \


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
