#include "tac/config.hpp"
#include "tac/exception.hpp"
#include <algorithm>
#include <cassert>
#include <string>
#include <string_view>


namespace tac
{

CharStream::CharStream(const char *str):
    dataStr_(str), iter_(dataStr_.begin())
{}


CharStream::CharStream(const std::string &str)
    : dataStr_(str), iter_(dataStr_.begin())
{}


CharStream::CharStream(const std::string_view &str)
    : dataStr_(str), iter_(dataStr_.begin())
{}


char CharStream::peek() const
{
    if (hasNext())
    {
        if (*iter_ == '\\')
            return *(iter_ + 1);
        else
            return *iter_;
    }
    return '\0';
}


char CharStream::rawPeek() const
{
    if (hasNext())
    {
        return *iter_;
    }
    return '\0';
}


bool CharStream::hasNext() const
{
    return iter_ != dataStr_.end();
}


void CharStream::skipWitespace()
{
    while (
        hasNext() &&
        (*iter_ == ' ' ||
         *iter_ == '\t' ||
         *iter_ == '\r')
    )
        ++iter_;
}


bool CharStream::isNewLine()
{
    if (rawPeek() == '\n')
        return true;
    else
        return false;
}

char CharStream::next()
{
    if (!hasNext())
        return '\0';

    char ch;
    if (*iter_ == '\\')
    {
        ch = *(iter_ + 1);
        iter_ += 2;
    }
    else
    {
        ch = *iter_;
        ++iter_;
    }

    return ch;
}


char CharStream::rawNext()
{
    if (!hasNext())
        return '\0';

    char ch = *iter_;
    ++iter_;

    return ch;
}


void CharStream::rollBack()
{
    if (iter_ != dataStr_.begin())
        --iter_;
    else
        throw TACException(TAC_PARSE_CONFIG_ERROR);
}


void CharStream::assertChar(char ch) const
{
    assert(ch == *iter_ && "parse config error");
}


std::string CharStream::parseKey()
{
    std::string key = "";
    while (hasNext() && peek() != ':')
    {
        key += next();
    }
    if (!hasNext())
        throw TACException(TAC_PARSE_CONFIG_ERROR);
    return key;
}


int CharStream::parseInt()
{
    int ret = 0;
    int sign = 1;
    char ch;

    if (rawPeek() == '-')
    {
        ch = rawNext();
        if (ch >= '0' && ch <= '9')
        {
            sign = -1;
        }
        else
        {
            // not a number
            rollBack();
            // just return something
            return 0;
        }
    }

    while(hasNext())
    {
        ch = rawPeek();
        if(ch >= '0' && ch <= '9')
        {
            ret *= 10;
            ret += ch - '0';
            rawNext();
        }
        else
        {
            break;
        }
    }
    return ret;
}


float CharStream::parseFloatAfterDot()
{
    float ret = 0;
    float denom = 10;
    char ch;

    while(hasNext())
    {
        ch = rawPeek();
        if (ch >= '0' && ch <= '9')
        {
            ret += static_cast<float>(ch - '0') / denom;
            denom *= 10;
            rawNext();
        }
        else
        {
            break;
        }
    }

    return ret;
}


void CharStream::skipUntilEndOfLine()
{
    char ch;
    while (hasNext() && (ch = rawPeek()) && ch != '\n')
        rawNext();
}

void Document::parse(const char *str)
{
    parse(str);
}


void Document::parse(const std::string &str)
{
    parse(str);
}



/*
a:
    b:
        c:d
        c1:d1
    e:
        c:d
        c1:d1
*/
void Document::parse(const std::string_view &str)
{
    CharStream cs(str);
    parse_doc(cs, this);
}


void Document::parse_doc(CharStream &cs, Value *currDoc)
{
    std::string key;
    while (cs.hasNext())
    {
        cs.skipWitespace();
        key = cs.parseKey();
        cs.skipWitespace();
        cs.assertChar(':');
        cs.rawNext(); // skip ':'
        cs.skipWitespace();

        // Value
        if (cs.isNewLine())
        {
            // sub document or array
            cs.skipWitespace();
            if (cs.rawPeek() == '-')
            {
                // array
                currDoc->insertMember(key, parse_array(cs));
            }
            else
            {
                // sub document
                Value value;
                parse_doc(cs, &value);
                currDoc->insertMember(key, std::move(value));
            }
        }
        else
        {
            // number or string
            currDoc->insertMember(key, parse_trivial(cs));
        }
    }
}


Value &&Document::parse_array(CharStream &cs)
{
    Value ret;

    while (cs.rawPeek() == '-')
    {
        cs.rawNext();           // skip '-'
        cs.assertChar(' '); // some space follow by '-'
        cs.skipWitespace();     // skip white space
        ret.push_back(parse_trivial(cs));
        cs.assertChar('\n');
        cs.rawNext();           // skip '/n'
        cs.skipWitespace();     // skip space at the begin of a line
    }

    return std::move(ret);
}


Value &&Document::parse_trivial(CharStream &cs)
{
    auto uv = cs.parseToEndOfLine();
    if (uv.type == CharStream::Int)
        return std::move(Value(uv.int_));
    else if (uv.type == CharStream::Float)
        return std::move(Value(uv.float_));
    else if (uv.type == CharStream::Str)
        return std::move(Value(std::move(uv.str_)));
    else
        throw TACException(TAC_PARSE_CONFIG_ERROR);
}

}
