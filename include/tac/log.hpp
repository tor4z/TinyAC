#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>


namespace tac
{

namespace 
{
inline std::string timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t seconds = tv.tv_sec;

    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << tm_time.tm_year + 1900
       << "/"
       << std::setfill('0') << std::setw(2) << tm_time.tm_mon + 1
       << "/"
       << std::setfill('0') << std::setw(2) << tm_time.tm_mday
       << " "
       << std::setfill('0') << std::setw(2) << tm_time.tm_hour
       << ":"
       << std::setfill('0') << std::setw(2) << tm_time.tm_min
       << ":"
       << std::setfill('0') << std::setw(2) << tm_time.tm_sec
       << "."
       << std::setprecision(6) << tv.tv_usec;
    return ss.str();
}


#define LOG_MAP(XX) \
    XX(DEBUG, "DEBUG") \
    XX(INFO, "INFO") \
    XX(WARN, "WARN") \
    XX(ERROR, "ERROR") \
    XX(FATAL, "FATAL")


enum LOG_LEVEL
{
#define MAKE_ENUM(log, name) LOG_##log,
    LOG_MAP(MAKE_ENUM)
#undef MAKE_ENUM
};


inline const char *logName(LOG_LEVEL level)
{
    static const char *logNameList[] =
    {
#define MAKE_NAME(log, name) name,
    LOG_MAP(MAKE_NAME)
#undef MAKE_NAME
    };

    return logNameList[level];
}


class Logger
{
public:
    Logger(LOG_LEVEL level, const char *file, int line, int err)
        : level_(level),
          file_(file),
          line_(line),
          err_(err)
    {}

    inline void operator<<(const std::string &str)
    {
        if (level_ == LOG_DEBUG)
#ifdef NDEBUG
            handleDebugInfoWarnError(str);
#endif
        else if (level_ == LOG_FATAL)
            handleFatal(str);
        else
            handleDebugInfoWarnError(str);
    }
private:
    const char *file_;
    const int line_;
    const int err_;
    LOG_LEVEL level_;

    std::string fmtMsg(const std::string &str)
    {
        std::stringstream ss;

        ss << "[" << logName(level_) << "] "
           << timestamp() << " " << file_ << ":" << line_
           << " " << str << std::endl;

        return ss.str();
    }

    inline void handleDebugInfoWarnError(const std::string &str)
    {
        if (err_ > 0)
        {
            // PERROR
            int tmp = errno;
            errno = err_;
            perror(fmtMsg(str).c_str());
            errno = tmp;
        }
        else
        {
            if (level_ <= LOG_WARN)
                std::cout << fmtMsg(str);
            else
                std::cerr << fmtMsg(str);
        }
    }

    inline void handleFatal(const std::string &str)
    {
        if (err_ > 0)
        {
            // PFATAL
            int tmp = errno;
            errno = err_;
            perror(fmtMsg(str).c_str());
            errno = tmp;
        }
        else
        {
            handleDebugInfoWarnError(str);
        }
        exit(level_);
    }
};
}


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DEBUG() Logger(LOG_DEBUG, __FILENAME__, __LINE__, -1)
#define INFO() Logger(LOG_INFO, __FILENAME__, __LINE__, -1)
#define WARN() Logger(LOG_WARN, __FILENAME__, __LINE__, -1)
#define ERROR() Logger(LOG_ERROR, __FILENAME__, __LINE__, -1)
#define FATAL() Logger(LOG_FATAL, __FILENAME__, __LINE__, -1)
#define PERROR() Logger(LOG_ERROR, __FILENAME__, __LINE__, errno) // errno always >= 1
#define PFATAL() Logger(LOG_FATAL, __FILENAME__, __LINE__, errno)

}
