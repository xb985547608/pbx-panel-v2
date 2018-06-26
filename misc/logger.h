#ifndef logger_h
#define logger_h

#include <cstdio>
#include <string>
#include "singleton.h"
#ifdef WIN32
#include <QMutex>
#else
#include <pthread.h>
#endif


#define LOG(level, fmt, args...) do { \
    if (!Logger::Instance()->shouldLog(level)) \
        break; \
    Logger::Instance()->log("[%s]%s(%d): ", Logger::getLevelName(level).c_str(), __FILE__, __LINE__); \
    Logger::Instance()->log(fmt, ##args); \
    } while(0)

class Logger : public Singleton<Logger>
{
 private :
	int mVerbose;
    static FILE* mOutput;
 public :
    enum _Verbose {
        None = 0,
        Error = 1,
        Warn = 2,
        Notice = 3,
        Debug = 4,
        DDebug = 5
    };
    Logger() : Singleton<Logger>("Logger") {};
    virtual ~Logger();
	bool initialise(int verbose = None, const char* path = NULL);
	void log(int verbose, const char* fmt, ...);
    void log(int verbose, std::string msg);
    void log(const char* fmt, ...);
    const char* getVerboseName(int level);
    bool shouldLog(int msgLevel) {
        return (mVerbose < msgLevel) ? false : true;
    }
    static std::string getLevelName(int level);
};

#endif
