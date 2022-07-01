#pragma once

#include "tac/exception.hpp"
#include <cmath>
#include <string>
#include <utility>
#include <vector>


namespace tac
{


class Member;


class CharStream
{
public:
    enum ValueType
    {
        None,
        Int,
        Float,
        Str
    };

    struct UnionValue
    {
        ValueType type;
        union{
            int int_;
            float float_;
            std::string str_;
        };

        UnionValue(ValueType t, int && d)
            : type(t), int_(std::move(d))
        {}
        UnionValue(ValueType t, float && d)
            : type(t), float_(std::move(d))
        {}
        UnionValue(ValueType t, std::string && d)
            : type(t), str_(std::move(d))
        {}

        ~UnionValue()
        {}
    };

    using Iter = std::string_view::iterator;

    CharStream(const char *str);
    CharStream(const std::string &str);
    CharStream(const std::string_view &str);

    char peek() const;
    char next();
    void rollBack();
    char rawPeek() const;
    char rawNext();
    bool hasNext() const;
    void skipWitespace();
    bool isNewLine();
    void assertChar(char ch) const;
    int parseInt();
    float parseFloatAfterDot();
    void skipUntilEndOfLine();

    UnionValue parseToEndOfLine()
    {
        ValueType type = None;
        const char *strBegin = &(*iter_);
        int sign = 1;
        char ch;

        int int_ = 0;
        float float_ = 0.0;
        std::string str_;

        ch = rawPeek();
        if (ch > '0' && ch < '9')
        {
            type = Int;
            int_ = parseInt();

            ch = rawPeek();
            if (ch == '.')
            {
                type = Float;
                float f = parseFloatAfterDot();
                float_ = static_cast<float>(int_) + f;

                ch = rawPeek();
                if (ch == 'e' || ch == 'E')
                {
                    int exp = parseInt();
                    float_ *= std::pow(10, parseInt());
                }
            }
            else if (ch == 'e' || ch == 'E')
            {
                    int exp = parseInt();
                    float_ = static_cast<float>(int_);
                    float_ *= std::pow(10, parseInt());
            }
        }

        // end of file or end of line
        if (!hasNext() || rawPeek() == '\n')
        {
            // int or float
            if (type == Int)
                return UnionValue(Int, std::move(int_));
            else
                return UnionValue(Float, std::move(float_));
        }
        else
        {
            type = Str;
            skipUntilEndOfLine();
            const char *strEnd = &(*iter_);
            str_.assign(strBegin, strEnd);
            return UnionValue(Str, std::move(str_));
            // string
        }
    }

    std::string parseKey();
private:
    const std::string_view dataStr_;
    Iter iter_;
};


class Value
{
public:
    enum Type
    {
        Doc,
        Int,
        Float,
        Str,
        List
    };

    Value(int val) :int_(val), type_(Int)
    {}

    Value(float val) :float_(val), type_(Float)
    {}

    Value(std::string &&val)
        :str_(Str, val), type_(Str)
    {}

    // for array
    Value()
    {}

    // for copy
    Value(const Value &value);

    // for move
    // Value()
    // {}

    ~Value();

    void push_back(Value &&value);
    const Value &operator[](const std::string &key) const;

    void insertMember(const std::string &key, Value &&value);
    Value &findValue();

private:
    template<typename T>
    class ObjectContainer_
    {
    public:
        template<typename ...Args>
        ObjectContainer_(Type type, Args&&... args)
            : type_(type), data_(std::move<Args>(args)...)
        {}
        Type type() const
        {
            return type_;
        }

        T data() const 
        {
            return data_;
        }
    private:
        Type type_;
        T data_;
    };

    using StrObject = ObjectContainer_<std::string>;
    using ListObject = ObjectContainer_<std::vector<Value>>;
    using DocObject = ObjectContainer_<std::vector<Member>>;

    union
    {
        int int_;
        float float_;
        StrObject str_;
        ListObject list_;
        DocObject doc_;
    };
    Type type_;
};


class Member
{
public:
private:
    std::string key_;
    Value value_;
};


class Document: public Value
{
public:
    void parse(const char *str);
    void parse(const std::string &str);
    void parse(const std::string_view &str);
    ~Document();
private:
    void parse_doc(CharStream &cs, Value *currDoc);
    Value &&parse_array(CharStream &cs);
    Value &&parse_trivial(CharStream &cs);
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
