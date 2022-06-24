#include "tac/exception.hpp"


namespace tac
{

static const char * getErrorMsg(TACError err)
{
    static const char *errorMsg[] =
    {
#define MAKE_ERR_MSG(e, s) s,
    EXCEPT_MAP(MAKE_ERR_MSG)
#undef MAKE_ERR_MSG
    };

    return errorMsg[static_cast<int>(err)];
}


TACException::TACException(TACError err): err_(err)
{}


const char *TACException::what() const noexcept
{
    return getErrorMsg(err_);
}


TACError TACException::err () const
{
    return err_;
}

}
